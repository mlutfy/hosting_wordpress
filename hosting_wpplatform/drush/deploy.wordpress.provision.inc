<?php

/**
 * @file
 * Implementation for provision-wordpress-deploy
 *
 * A lot was copied from platform/deploy.provision.inc
 */

/**
 * Make sure we have a valid site being deployed, and that the file being deployed from exists.
 *
 * Implementation of drush_hook_COMMAND_validate().
 */
function drush_provision_wordpress_deploy_validate($backup_file = NULL) {
  $exists = provision_file()->exists($backup_file)
    ->succeed('Deploying site from @path')
    ->fail('Could not find backup file @path', 'PROVISION_BACKUP_NOT_FOUND')
    ->status();
  if ($exists) {
    drush_set_option('backup_file', $backup_file);
  }

  $exists = provision_file()->exists(d()->site_path)
    ->succeed('Replacing the existing site at @path')
    ->status();

  if ($exists) {
    drush_set_option('extract_path', d()->site_path . '.restore');
    drush_set_option('old_db_name', drush_get_option('db_name', ''));
  }
  else {
    drush_set_option('extract_path', d()->site_path);
  }

  drush_set_option('deploy_replace_site', $exists);
}

/**
 * Remove the extracted site directory.
 *
 * Implementation of drush_hook_pre_COMMAND_rollback().
 *
 * @todo Not tested.
 */
function drush_provision_drupal_pre_provision_deploy_rollback() {
  if (drush_get_option('deploy_replace_site', FALSE)) {
    if (drush_get_option('site_dirs_swapped', FALSE)) {

      // Swap the site directories back if necessary.
      $old = d()->site_path . '.restore';
      $new = d()->site_path;

      provision_file()->switch_paths($old, $new)
        ->succeed('Swapping out the @path1 and @path2 directories was successful.')
        ->fail('Swapping the @path1 and @path2 directories has failed.', 'DRUSH_PERM_ERROR');

      // @todo?
      // _provision_drupal_create_settings_file();
      // provision_save_site_data();
    }
  }

  if (provision_file()->exists(drush_get_option('extract_path'))->status()) {
    _provision_recursive_delete(drush_get_option('extract_path'));
  }
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_provision_wordpress_deploy($backup_file) {
  drush_log(dt('provision_wordpress/deploy: extract !file to !path', ['!file' => $backup_file, '!path' => drush_get_option('extract_path')]), 'ok');

  $extracted = provision_file()->extract($backup_file, drush_get_option('extract_path'))
    ->succeed('Successfully extracted the contents of @path')
    ->fail('Failed to extract the contents of @path to @target', 'PROVISION_BACKUP_EXTRACTION_FAILED')
    ->status();

  if ($extracted) {
    // Make sure the files in the files directory are accessible by the web server.
    provision_file()->chgrp(drush_get_option('extract_path') . '/wp-content', d('@server_master')->web_group, TRUE)
      ->succeed('Changed group ownership of files in <code>@path</code> to @gid')
      ->fail('Could not change group ownership of files in <code>@path</code> to @gid');

    drush_shell_exec("sudo /usr/local/bin/fix-wordpress-permissions.sh --site-path=%s", drush_get_option('extract_path'));

    if (drush_get_option('deploy_replace_site', FALSE)) {
      $old = d()->site_path . '.restore';
      $new = d()->site_path;

      $swapped = provision_file()->switch_paths($old, $new)
        ->succeed('Swapping out the @path1 and @path2 directories was successful.')
        ->fail('Swapping the @path1 and @path2 directories has failed.', 'DRUSH_PERM_ERROR')
        ->status();

      if ($swapped) {
        drush_set_option('site_dirs_swapped', TRUE);
      }
      else {
        return FALSE;
      }
    }

    // We have already created a new database. Save the info to the config files.
    // @todo ?
    // provision_prepare_environment();
    // provision_save_site_data();

    // Load the newly created information, including re-loading the new db creds.
    // _provision_drupal_create_settings_file();
  }

  d()->service('db')->create_site_database();

  // FIXME Duplicates code from install.wordpress.provision.inc
  $_SERVER['db_type'] = drush_get_option('db_type', NULL, 'site');
  $_SERVER['db_host'] = drush_get_option('db_host', NULL, 'site');
  $_SERVER['db_port'] = drush_get_option('db_port', NULL, 'site');
  $_SERVER['db_passwd'] = drush_get_option('db_passwd', NULL, 'site');
  $_SERVER['db_name'] = drush_get_option('db_name', NULL, 'site');
  $_SERVER['db_user'] = drush_get_option('db_user', NULL, 'site');
  $_SERVER['wp_content_dir'] = d()->site_path . '/wp-content';
  $_SERVER['wp_content_url'] = $protocol . '://' . d()->uri . '/sites/' . d()->uri . '/wp-content';

  // Then we add it in the wpsite context
  drush_set_option('db_type', $_SERVER['db_type'], 'wpsite');
  drush_set_option('db_host', $_SERVER['db_host'], 'wpsite');
  drush_set_option('db_port', $_SERVER['db_port'], 'wpsite');
  drush_set_option('db_passwd', $_SERVER['db_passwd'], 'wpsite');
  drush_set_option('db_name', $_SERVER['db_name'], 'wpsite');
  drush_set_option('db_user', $_SERVER['db_user'], 'wpsite');
  drush_set_option('wp_content_dir', $_SERVER['wp_content_dir'], 'wpsite');
  drush_set_option('wp_content_url', $_SERVER['wp_content_url'], 'wpsite');


  d()->service('http')->create_config('site');
  $config = new Provision_Config_Drushrc_wpsite(d()->name);
  $config->write();

  d()->service('db')->import_site_database();
  provision_file()->unlink(d()->site_path . '/database.sql');

  // _provision_drupal_maintain_aliases();
  // _provision_drupal_create_directories();

  // This was in the "post" hook
  // Call the drush updatedb command.
  // provision_backend_invoke(d()->name, 'updatedb');

  // Remove the restore directory.
  if (!drush_get_error() && drush_get_option('deploy_replace_site', FALSE)) {
    drush_shell_exec("sudo /usr/local/bin/fix-wordpress-permissions.sh --site-path=%s", drush_get_option('extract_path'));
    _provision_recursive_delete(drush_get_option('extract_path'));
    drush_log(dt('WordPress: deleted !path', ['!path' => drush_get_option('extract_path')], 'ok');
  }

  // Remove the old database.
  if (!drush_get_error() && drush_get_option('deploy_replace_site', FALSE)) {
    if ($old_database = drush_get_option('old_db_name', '')) {
      if (!d()->service('db')->drop_database($old_database)) {
        drush_log(dt('Failed to drop database @old_database', array('@old_database' => $old_database)), 'warning');
      }
      else {
        drush_log(dt('Dropped the old database (@old_database).', array('@old_database' => $old_database)), 'info');
      }
    }
  }
}

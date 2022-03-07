<?php

/**
 * @file
 * Implementation for provision-wordpress-clone
 * A lot was copied from clone/hosting_clone.drush.inc
 */

/**
 * Make a backup before making any changes, and add extract the file we are restoring from
 */
function drush_provision_wordpress_pre_provision_wordpress_clone($new_name, $platform = null) {
  drush_invoke('provision-wordpress-backup');
}

/**
 * Remove the extracted site directory
 */
function drush_provision_wordpress_pre_provision_wordpress_clone_rollback($new_name, $platform = null) {
  $success =  provision_file()->unlink(drush_get_option('backup_file'))
    ->succeed('Removed unused clone site package')
    ->fail('Could not remove unused clone site package');
}

/**
 * Switch the clone directories around now that we have the new db installed
 */
function drush_provision_wordpress_clone($new_name = NULL, $platform = null) {
  drush_set_option('old_platform', d()->wpplatform->name);

  // If the site is cloned between platforms and not just in the same platform,
  // we should update the info collected about source and target platform first.
  if (!is_null(d($platform)->name) && (d($platform)->name != d()->wpplatform->name)) {
    provision_backend_invoke('@hostmaster', 'hosting-task', array(d()->wpplatform->name, 'verify'), array('force' => TRUE));
    sleep(5); // A small trick to avoid high load and race conditions.
    provision_backend_invoke('@hostmaster', 'hosting-task', array(d($platform)->name, 'verify'), array('force' => TRUE));
    sleep(5); // A small trick to avoid high load and race conditions.
  }
  // We should update also the info collected about the site before running clone.
  $local_uri_verify = '@' . d()->uri;
  provision_backend_invoke('@hostmaster', 'hosting-task', array($local_uri_verify, 'wordpress-verify'), array('force' => TRUE));
  sleep(5); // A small trick to avoid high load and race conditions.

  $options = d()->options;
  $options['uri'] = ltrim($new_name, '@');
  $hash_name = drush_get_option('#name') ? '#name' : 'name';
  $options[$hash_name] = $new_name;
  $options['wpplatform'] = (isset($platform)) ? $platform : $options['wpplatform'];
  $options['root'] = d($options['wpplatform'])->root;
  $options['aliases'] = [];
  $options['redirection'] = 0;

  // XXX: right now we just drop SSL configuration when cloning, because
  // we do not check if the site name change is compatible with the
  // certificate. This can be removed when we a) check for wildcard
  // certs and b) allow the user to change the cert from the UI.
  $options['ssl_enabled'] = 0;
  unset($options['ssl_key']);

  // Do not copy CDN aliases to the cloned site --CDN vhost
  $options['cdn'] = 0;
  unset($options['cdn']);

  if ($db_server = drush_get_option('new_db_server', FALSE)) {
    $options['db_server'] = $db_server;
  }

  drush_invoke_process('@none', 'provision-save', array($new_name), $options);
  // note that we reset the aliases so they don't conflict with the original site

  // Build an options array to pass to the deploy task. This allows injection
  // of data into the deploy task. See: https://www.drupal.org/node/2471805
  $deploy_options = array(
    'old_uri' => d()->uri,
    'strict' => 0,
  );

  // drush_command_invoke_all_ref('provision_deploy_options_alter', $deploy_options, 'clone');
  provision_backend_invoke($new_name, 'provision-wordpress-deploy', array(drush_get_option('backup_file')), $deploy_options);

  if (!drush_get_error()) {
    // Verify the newly cloned site.
    provision_backend_invoke($new_name, 'provision-wordpress-verify');
    sleep(5); // A small trick to avoid high load and race conditions.
    // Verify again also original site via frontend to avoid issue #1004526
    provision_backend_invoke('@hostmaster', 'hosting-task', array($local_uri_verify, 'wordpress-verify'), array('force' => TRUE));
  }

  provision_reload_config('wpsite', d()->site_path . '/drushrc.php');
}

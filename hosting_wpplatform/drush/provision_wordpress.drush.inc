<?php

/**
 * @file
 *   Aegir service autoloading function and switch based on version and include
 *   the appropriate files to handle install, verify, &c.
 */

require_once('provision_wordpress.inc');

# require_once(dirname(__FILE__) . '/deploy.provision.inc');
# [done] require_once(dirname(__FILE__) . '/install.provision.inc');
# require_once(dirname(__FILE__) . '/migrate.provision.inc');

require_once(dirname(__FILE__) . '/verify.provision.inc');

/**
 * Implements hook_drush_init().
 */
function provision_wordpress_drush_init() {
  // Register our service classes for autoloading.
  // FIXME This seems to work better now, but if there are issues,
  // comment this call, and uncomment the other global call.
  provision_wordpress_provision_register_autoload();
}

function drush_provision_wordpress_pre_hosting_task() {
  if (module_exists('hosting_alias')) {
    $task = &drush_get_context('HOSTING_TASK');
    if ($task->ref->type == 'wpsite' && function_exists('hosting_alias_get_aliases')) {
      // Copied from hosting_alias
      $aliases = implode(',', hosting_alias_get_aliases($task->ref));
      $task->context_options['aliases'] = ($aliases) ? $aliases : 'null';

      $redirection = db_query("SELECT redirection FROM {hosting_site_alias} WHERE vid = :vid", array(':vid' => $task->ref->vid))->fetchField();
      $task->context_options['redirection'] = ($redirection) ? $redirection : 'null';
    }
  }
  if ($task->ref->type == 'wpsite' && $task->task_type == 'wordpress-clone') {
    $task->args[2] = '@' . hosting_site_get_domain($task->task_args['new_uri']);
    $platform = node_load($task->task_args['target_platform']);
    $task->args[3] = hosting_context_name($platform->nid);

    if ($task->ref->db_server != $task->task_args['new_db_server']) {
      $task->options['new_db_server'] = hosting_context_name($task->task_args['new_db_server']);
    }

    if (module_exists('hosting_alias')) {
      // Why would you copy then when cloning?
      // $task->options['aliases'] = str_replace("\n", ",", $task->task_args['aliases']);
      // $task->options['redirection'] = $task->task_args['redirection'];
    }

    // This was in clone post_task
    $target = $task->task_args['target_platform'];
    $clone = new stdClass();
    // copy some of the settings of the cloned site ($task->ref) to $clone
    foreach (['type', 'status', 'uid', 'comment', 'promote', 'name', 'format', 'client', 'db_server', 'profile', 'site_status', 'site_language'] as $field) {
      $clone->$field = $task->ref->$field;
    }
    $clone->title = hosting_site_clean_domain($task->task_args['new_uri']);
    $clone->hosting_name = hosting_site_get_domain($clone->title);
    $clone->wpplatform = $target;
    $clone->import = 1; // make sure the site doesn't reinstall...
    $clone->verified = 0; // ... and it does verify
    $clone->aliases = isset($task->task_args['aliases']) ? $task->task_args['aliases'] : [];
    $clone->redirection = isset($task->task_args['redirection']) ? $task->task_args['redirection'] : FALSE;

    if ($clone->db_server != $task->task_args['new_db_server']) {
      $clone->db_server = $task->task_args['new_db_server'];
    }

    node_save($clone);

    // Record the backup created during clone.
    $task->task_args['description'] = t('Pre-clone backup');
    hosting_wpsite_post_hosting_wordpress_backup_task($task, $data);
  }

}

/**
 * Implements hook_drush_context_import().
 *
 * If we're importing a site, then import the aliases and redirection values.
 */
function hosting_provision_drush_context_import($context, &$node) {
  if ($context->type == 'wpsite') {
    // Copied from hosting_alias
    if (function_exists('hosting_alias_get_aliases')) {
      $node->aliases = $context->aliases;
      $node->redirection = $context->redirection;
    }
  }
}

/**
 * Register our directory as a place to find Provision classes.
 *
 * This allows Provision to autoload our classes, so that we don't need to
 * specifically include the files before we use the class.
 */
function provision_wordpress_provision_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    $list = drush_commandfile_list();
    $provision_dir = dirname($list['provision']);
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Implements hook_provision_services().
 */
function provision_wordpress_provision_services() {
  provision_wordpress_provision_register_autoload();
  return array('wpsite' => NULL, 'wpplatform' => NULL);
}

/**
 * Implementation of hook_drush_command().
 */
function provision_wordpress_drush_command() {
  $items['provision-wordpress-install'] = array(
    'description' => 'Install WordPress.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'options' => array(
      'client_email' => dt('The email address of the client to use.'),
    ),
  );
  $items['provision-wordpress-verify'] = array(
    'description' => 'Verify WordPress.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['provision-wordpress-enable'] = array(
    'description' => 'Enable a WordPress site',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['provision-wordpress-disable'] = array(
    'description' => 'Disable a WordPress site',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['provision-wordpress-delete'] = array(
    'description' => 'Delete WordPress.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['provision-wordpress-cli'] = array(
    'description' => 'Run a wp-cli command.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('wp-cli', 'wp'),
  );
  $items['provision-wordpress-backup'] = array(
    'optional arguments' => array('backup-file' => dt('The file to save the backup to. This will be a gzipped tarball.')),
    'options' => array(
      'provision_backup_suffix' => dt('Set the extension for the backupfile, compression is derived from this (default: ".tar.gz"'),
    ),
    'description' => dt('Generate a back up for the site.'),
    'allow-additional-options' => TRUE,
    'examples' => array(
      'drush @site provision-wordpress-backup' => 'Back up the site as defined by the site Drush alias generated with provision-save.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH
  );
  $items['provision-wordpress-restore'] = array(
    'description' => 'Restore the site to a previous backup. This will also generate a backup of the site as it was.',
    'arguments' => array(
      'site_backup.tar.gz' => dt('The backup to restore the site to.'),
    ),
    'allow-additional-options' => TRUE,
    'examples' => array(
      'drush @site provision-wordpress-restore ~/backups/some_site.tar.gz' => 'Restore the site to the backup in ~/backups/some_site.tar.gz.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH
  );
  $items['provision-wordpress-deploy'] = array(
    'description' => 'Deploy an existing backup to a new url.',
    'arguments' => array(
      'site_backup.tar.gz' => dt('The backup to deploy.'),
    ),
    'options' => array(
      'old_uri' => dt('Old site uri to replace in references to sites/example.com/files/ in the database content.)'),
    ),
    'allow-additional-options' => TRUE,
    'examples' => array(
      'drush @site provision-wordpress-deploy ~/backups/some_site.tar.gz' => 'Deploy the site as defined by the site Drush alias, from the backup in ~/backups/some_site.tar.gz.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH
  );
  $items['provision-wordpress-clone'] = array(
    'description' => 'Clone a site between WP platforms.',
    'arguments' => array(
      '@new_site' => dt('The Drush alias of the new site as generated by provision-save.'),
      '@platform_name' => dt('The Drush alias of the platform to clone the site onto.')),
    'options' => array(
      'profile' => dt('The Drupal profile to use.')),
    'allow-additional-options' => TRUE,
    'examples' => array(
      'drush @site provision-wordpress-clone @new_site @platform_name' => 'Clone the original site to the new site on a platform',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH
  );
  $items['provision-wordpress-import'] = array(
    'description' => dt('Turn an already running WP site into a provisioned site.'),
    'options' => array(
      'client_email' => dt('The email address of the client to use.'),
    ),
    'allow-additional-options' => TRUE,
    'examples' => array(
      'drush @site provision-wordpress-import' => 'Import the WP site as defined by the site Drush alias generated with provision-save.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH
  );

  return $items;
}

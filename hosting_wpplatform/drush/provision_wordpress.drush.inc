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

// FIXME: this needs to run very early in the bootstrap, otherwise:
// PHP Fatal error:  Class 'Provision_Context_wpplatform' not found in /usr/share/drush/commands/provision/provision.context.inc on line 127
// UPDATE: this seems to be fixed now, but no idea what fixed it.
# provision_wordpress_provision_register_autoload();

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
  );
  $items['provision-wordpress-verify'] = array(
    'description' => 'Verify WordPress.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['provision-wordpress-cli'] = array(
    'description' => 'Run a wp-cli command.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('wp-cli', 'wp'),
  );

  return $items;
}

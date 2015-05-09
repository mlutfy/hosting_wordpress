<?php

/**
 * @file
 *   Aegir service autoloading function and switch based on version and include
 *   the appropriate files to handle install, verify, &c.
 */

# require_once('provision_wordpress.inc');
# require_once(dirname(__FILE__) . '/deploy.provision.inc');
# require_once(dirname(__FILE__) . '/install.provision.inc');
# require_once(dirname(__FILE__) . '/migrate.provision.inc');
# require_once(dirname(__FILE__) . '/verify.provision.inc');

/**
 * Implements hook_drush_init().
 */
function provision_wordpress_drush_init() {
  // Register our service classes for autoloading.
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
    include_once($provision_dir . '/provision.inc');
    include_once($provision_dir . '/provision.service.inc');
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}


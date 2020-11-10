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

  if (d()->type == 'wpsite') {
    $command = drush_get_command();

    if (empty($command) || ! isset($command['command'])) {
      return;
    }

    // Bootstrap WordPress, but for now only if doing a verify.
    // Some submodules, such as hosting_civicrm, check to see if their plugin is enabled or not.
    // When doing some commands such as wp-cli, wp-cli takes care of bootstrapping WordPress.
    //
    // And for what it's worth, this is probably not the best way to bootstrap WP.
    // wp-cli has a much more complex wp-settings.php that we should probably use as inspiration.
    if (in_array($command['command'], array('provision-verify', 'provision-wordpress-verify'))) {
      $path = d()->root . '/wp-load.php';
      require_once($path);
    }
  }
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

  return $items;
}

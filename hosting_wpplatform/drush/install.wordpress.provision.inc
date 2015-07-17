<?php

/**
 * Check that we are trying to install a new site, and a new site only
 */
function drush_provision_wordpress_install_validate() {
  drush_log('WordPress: starting drush_provision_wordpress_install_validate', 'ok');

  if (!d()->uri) {
    return drush_set_error("PROVISION_URL_REQUIRED", dt("You need to specify a valid url to install a site"));
  }

/* TODO
  if (_provision_drupal_site_exists()) {
    return drush_set_error('PROVISION_SITE_INSTALLED');
  }
*/

  drush_log('WordPress: finished drush_provision_wordpress_install_validate', 'ok');
}

/**
 * Implements the provision-wordpress-install command.
 *
 * Mostly copies /usr/share/drush/commands/provision/platform/reset.login.provision.inc
 * but for uid = 2.
 */
function drush_provision_wordpress_install() {
  drush_log('WordPress: starting drush_provision_wordpress_install', 'ok');

  // d()->service_subscribe('platform', d()->wpplatform);
  d()->service_subscribe('server', '@server_master'); // FIXME hardcoded name?
  d()->service_subscribe('http', '@server_master'); // FIXME hardcoded name?
  d()->service_subscribe('db', d()->db_server->name);

  d()->service('db')->create_site_database();
  d()->service('http')->create_config('site');

  // FIXME .. this relies on something something that might be removed
  // from provision/db/Provision/Service/db.php.
  //
  // However, since we cloak the db setings in wp-config.php, and that
  // is also the simplest way to do multi-platform, ideally we should
  // have a proper Aegir API to fetch the DB credentials.
  $_SERVER['db_type'] = drush_get_option('db_type', NULL, 'site');
  $_SERVER['db_host'] = drush_get_option('db_host', NULL, 'site');
  $_SERVER['db_port'] = drush_get_option('db_port', NULL, 'site');
  $_SERVER['db_passwd'] = drush_get_option('db_passwd', NULL, 'site');
  $_SERVER['db_name'] = drush_get_option('db_name', NULL, 'site');
  $_SERVER['db_user'] = drush_get_option('db_user', NULL, 'site');

  $_SERVER['wp_content_dir'] = d()->site_path . '/wp-content';

  // Then we add it in the wpsite context
  drush_set_option('db_type', $_SERVER['db_type'], 'wpsite');
  drush_set_option('db_host', $_SERVER['db_host'], 'wpsite');
  drush_set_option('db_port', $_SERVER['db_port'], 'wpsite');
  drush_set_option('db_passwd', $_SERVER['db_passwd'], 'wpsite');
  drush_set_option('db_name', $_SERVER['db_name'], 'wpsite');
  drush_set_option('db_user', $_SERVER['db_user'], 'wpsite');

  // And now save the wpsite context to disk (drushrc.php).
  $config = new Provision_Config_Drushrc_wpsite(d()->name);
  $config->write();

  // Directory permissions
  provision_wordpress_setup_directories();

  // Run the WordPress installer
  $admin_email = drush_get_option('client_email', 'fixme@example.org');

/*
  // Wishful thinking (but this mysteriously fails because wp-cli is not correctly initialized, see wpsite service):
  WP_CLI::run_command(array('core', 'install'), array(
    'url' => d()->uri,
    'title' => d()->uri,
    'admin_user' => 'admin',
    'admin_email' => $admin_email,
    'admin_password' => 'changeme', // FIXME TODO
  ));
*/

  // This helps with wp-cli require_maybe() statements
  global $wp_version;

  $GLOBALS['argv'] = array(
    'wp',
    '--path=' . d()->root . '/',
    'core',
    'install',
    '--url=' . d()->uri,
    '--title=' . d()->uri,
    '--admin_user=admin',
    '--admin_email=' . $admin_email,
    '--admin_password=changeme', // FIXME
  );

  define('WP_CLI_ROOT', '/var/aegir/.composer/vendor/wp-cli/wp-cli');
  require_once WP_CLI_ROOT . '/php/wp-cli.php';

  d()->service('http')->restart();

  drush_log('WordPress: finished drush_provision_wordpress_install', 'ok');
}

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
  d()->service_subscribe('server', '@server_master');
  d()->service_subscribe('http', '@server_master');
  d()->service_subscribe('db', '@server_127.0.0.1');

  d()->service('db')->create_site_database();
  d()->service('http')->create_config('site');

  // FIXME .. this relies on a hack, and surely there are better ways.
  $_SERVER['db_type'] = drush_get_option('db_type', NULL, 'site');
  $_SERVER['db_host'] = drush_get_option('db_host', NULL, 'site');
  $_SERVER['db_port'] = drush_get_option('db_port', NULL, 'site');
  $_SERVER['db_passwd'] = drush_get_option('db_passwd', NULL, 'site');
  $_SERVER['db_name'] = drush_get_option('db_name', NULL, 'site');
  $_SERVER['db_user'] = drush_get_option('db_user', NULL, 'site');

  $GLOBALS['argv'] = array(
    'wp',
    '--path=' . d()->root . '/',
    'core',
    'install',
    '--url=' . d()->uri,
    '--title=' . d()->uri,
    '--admin_user=admin',
    '--admin_email=mathieu@bidon.ca', // FIXME
    '--admin_password=changeme', // FIXME
  );

  define('WP_CLI_ROOT', '/var/aegir/lib/wp-cli');
  require_once '/var/aegir/lib/wp-cli/php/wp-cli.php';

  drush_log('WordPress: finished drush_provision_wordpress_install', 'ok');
}

<?php

/**
 * The wordpress service class.
 */
class Provision_Service_wpsite extends Provision_Service {
  public $service = 'wpsite';

  static function subscribe_wpsite($context) {
    drush_log('WordPress: service subscribe_wpsite');
    $context->is_oid('wpsite');

    // Copied from provision/db/Provision/Service/db.php
    $context->setProperty('db_server', '@server_master');
    $context->is_oid('db_server');
    $context->service_subscribe('db', $context->db_server->name);

    // Required for wordpress-delete (to delete the vhost file).
    if (!empty($context->web_server->name)) {
      $context->service_subscribe('http', $context->web_server->name);
    }

    // Drushrc needs this to find the drushrc.php file
    $context->setProperty('site_path');

    // Load the drushrc.
    // Since wpsite is not a valid drush context, we need to do it manually.
    // This will load variables in $_SERVER.
    $config = new Provision_Config_Drushrc_wpsite($context->name);
    $filename = $config->filename();

    if ($filename && file_exists($filename)) {
      drush_log(dt('WordPress: loading !file', array('!file' => $filename)));
      global $options;

      include($filename);

      // This is necessary for deleting the DB in delete.wordpress.provision.inc
      foreach (array('db_type', 'db_host', 'db_port', 'db_passwd', 'db_name', 'db_user', 'wp_content_dir', 'wp_content_url') as $x) {
        drush_set_option($x, $options[$x], 'site');
      }
    }

    // Setup the constants for wp-cli
    // FIXME: doesn't work for the installation, hard to debug.
    // self::setup_wpcli_environment();
  }

  static function subscribe_wpplatform($context) {
    $context->is_oid('wpplatform');

    // Copied from provision/http/Provision/Service/http.php
    // Not sure if necessary.
/*
    $context->setProperty('web_server', $context->web_server->name);
    $context->is_oid('web_server');
    $context->service_subscribe('http', $context->web_server->name);
*/

    // Copied from provision/db/Provision/Service/db.php
    $context->setProperty('db_server', '@server_master');
    $context->is_oid('db_server');
    $context->service_subscribe('db', $context->db_server->name);
  }

  /**
   * Helper function to load wp-cli.
   * Code mostly copied from wp-cli/php/wp-cli.php
   *
   * FIXME: not used at the moment. Installation doesn't work using this method.
   * Hard to debug.
   */
  function setup_wpcli_environment() {
    if (defined('WP_CLI_ROOT')) {
      drush_log('WordPress: setup_wpcli_environment already initialized.');
      return;
    }

    define('WP_CLI_ROOT', '/var/aegir/.composer/vendor/wp-cli/wp-cli');

    // This is from WP_CLI/Runner.php set_wp_root()
    define('ABSPATH', d()->wpplatform->root . '/');
    $_SERVER['DOCUMENT_ROOT'] = realpath(ABSPATH);

    // Can be used by plugins/themes to check if WP-CLI is running or not
    define('WP_CLI', true);
    define('WP_CLI_VERSION', trim(file_get_contents(WP_CLI_ROOT . '/VERSION')));

    # define('WP_INSTALLING', true);
    # define('WP_SETUP_CONFIG', true);

    // Set common headers, to prevent warnings from plugins
    $_SERVER['SERVER_PROTOCOL'] = 'HTTP/1.0';
    $_SERVER['HTTP_USER_AGENT'] = '';
    $_SERVER['REQUEST_METHOD'] = 'GET';
    $_SERVER['REMOTE_ADDR'] = '127.0.0.1';

    include WP_CLI_ROOT . '/php/utils.php';
    include WP_CLI_ROOT . '/php/dispatcher.php';
    include WP_CLI_ROOT . '/php/class-wp-cli.php';
    include WP_CLI_ROOT . '/php/class-wp-cli-command.php';

    // Try to catch warnings during installation.
    # $logger = new Provision_Loggers_Drush(FALSE);
    # \WP_CLI::set_logger($logger);

    \WP_CLI\Utils\load_dependencies();

    WP_CLI::get_runner()->before_wp_load();

    // Load wp-config.php code, in the global scope
    # eval(WP_CLI::get_runner()->get_wp_config_code());

    // Load Core, mu-plugins, plugins, themes etc.
    # This should not be done unless we are installed, and DB creds are loaded.
    # FIXME for db cli.
    # require WP_CLI_ROOT . '/php/wp-settings-cli.php';

    // Fix memory limit. See http://core.trac.wordpress.org/ticket/14889
    @ini_set( 'memory_limit', -1 );

    // Load all the admin APIs, for convenience
    # [ML] FIXME probably required..
    # require ABSPATH . 'wp-admin/includes/admin.php';

    drush_log('WordPress: setup_wpcli_environment initialization complete.', 'ok');
  }
}

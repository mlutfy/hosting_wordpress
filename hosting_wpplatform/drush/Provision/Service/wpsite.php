<?php

/**
 * The wordpress service class.
 */
class Provision_Service_wpsite extends Provision_Service {
  public $service = 'wpsite';

  static function subscribe_wpsite($context) {
    drush_log('WordPress: service subscribe_wpsite');
    $context->is_oid('wpsite');

    // Copied from provision/http/Provision/Service/http.php
    // Not sure if necessary.
    $context->setProperty('web_server', '@server_master');
    $context->is_oid('web_server');
    $context->service_subscribe('http', $context->web_server->name);

    // Copied from provision/db/Provision/Service/db.php
    $context->setProperty('db_server', '@server_master');
    $context->is_oid('db_server');
    $context->service_subscribe('db', $context->db_server->name);

    // Drushrc needs this to find the drushrc.php file
    $context->setProperty('site_path');

    // Load the drushrc.
    // Since wpsite is not a valid drush context, we need to do it manually.
    // This will load variables in $_SERVER.
    // We do not bother with $options for now, but we probably should?
    $config = new Provision_Config_Drushrc_wpsite($context->name);
    $filename = $config->filename();

    if ($filename && file_exists($filename)) {
      drush_log(dt('WordPress: loading !file', array('!file' => $filename)));
      include($filename);
    }
  }

  static function subscribe_wpplatform($context) {
    $context->is_oid('wpplatform');

    // Copied from provision/http/Provision/Service/http.php
    // Not sure if necessary.
    $context->setProperty('web_server', '@server_master');
    $context->is_oid('web_server');
    $context->service_subscribe('http', $context->web_server->name);

    // Copied from provision/db/Provision/Service/db.php
    $context->setProperty('db_server', '@server_master');
    $context->is_oid('db_server');
    $context->service_subscribe('db', $context->db_server->name);
  }
}

<?php

/**
 * The wordpress service class.
 */
class Provision_Service_wpsite extends Provision_Service {
  public $service = 'wpsite';

  /**
   *
   */
  static function subscribe_site($context) {
    // FIXME: not necessary?
    drush_log('SERVICE SITE CONTEXT: ' . print_r($context, 1), 'ok');
    $context->service_subscribe('db', $context->db_server->name);
  }

  static function subscribe_wpsite($context) {
    drush_log('SERVICE WPSITE CONTEXT: ' . print_r($context, 1), 'ok');
    $context->is_oid('wpsite');
  }

  static function subscribe_wpplatform($context) {
    drush_log('SERVICE WPPLATFORM CONTEXT: ' . print_r($context, 1), 'ok');
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

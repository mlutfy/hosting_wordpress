<?php

/**
 * The wordpress platform service class.
 */
class Provision_Service_wpplatform extends Provision_Service {
  public $service = 'wpplatform';

  function init_wpplatform() {
    // FIXME: this isn't called, because we define the service as wpplatform=>NULL
    // in hook_provision_services().. but if we define a 'default' service, we get:
    // Fatal error: Call to a member function setContext() on null in /usr/share/drush/commands/provision/Provision/Context/server.php on line 119
    drush_log('WordPress: wpplatform service init_wpplatform', 'ok');
    $this->configs['wpplatform'] = array('Provision_Config_wpplatform');
  }

  function verify_wpplatform_cmd() {
    drush_log('WordPress: wpplatform service verify_wpplatform_cmd', 'ok');
    $this->create_config($this->context->type);
    $this->parse_configs();
  }

  static function subscribe_wpplatform($context) {
    drush_log('WordPress: wpplatform subscribe_wpplatform', 'ok');
    $context->is_oid('wpplatform');

    // Copied from provision/http/Provision/Service/http.php
    // Not sure if necessary.
    $context->setProperty('web_server', '@server_master');
    $context->is_oid('web_server');
    $context->service_subscribe('http', $context->web_server->name);
  }
}

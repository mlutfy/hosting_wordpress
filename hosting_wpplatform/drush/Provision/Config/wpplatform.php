<?php

/**
 * Base class for WordPress platform config files (wp-config.php).
 *
 */
class Provision_Config_wpplatform extends Provision_Config {
  public $template = 'wp-config.tpl.php';
  public $description = 'wpplatform configuration file';

  function filename() {
    $filename = $this->context->root . '/wp-config.php';
    drush_log(dt("Wordpress: Provision_Config_wpplatform filename = !file", array('!file' => $filename)), 'ok');
    return $filename;
  }

  function write() {
    parent::write();

    // data['server'] below is not set (this was copied from http service, no probably needs adapting?)
    // $this->data['server']->sync($this->filename());
  }

  function unlink() {
    parent::unlink();
    // $this->data['server']->sync($this->filename());
  }

  function process() {
    parent::process();
    $this->data['extra_config'] = "# Hello world from Provision_Config_wpplatform\n";
  }
}

<?php
/**
 * @file
 * Provides the Provision_Config_Drushrc_wpsite class.
 */

/**
 * Class for writing $platform/sites/$url/drushrc.php files. (FIXME)
 */
class Provision_Config_Drushrc_wpsite extends Provision_Config_Drushrc {
  protected $context_name = 'wpsite';
  public $template = 'provision_drushrc_wpsite.tpl.php';
  public $description = 'wpsite Drush configuration file';

  function filename() {
    return $this->site_path . '/drushrc.php';
  }
}

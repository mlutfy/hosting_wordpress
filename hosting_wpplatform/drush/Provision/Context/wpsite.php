<?php

/**
 * @file WP Provision named context site class.
 */

class Provision_Context_wpsite extends Provision_Context {
  public $parent_key = 'wpplatform';

  static function option_documentation() {
    return array(
      'wpplatform' => 'site: the wpplatform the wpsite is run on',
      'platform' => 'site: the platform the wpsite is run on (compat for Aegir)',
      'db_server' => 'site: the db server the wpsite is run on',
      'uri' => 'site: example.com URI, no http:// or trailing /',
      'language' => 'site: site language; default en',
      'aliases' => 'site: comma-separated URIs',
      'redirection' => 'site: boolean for whether --aliases should redirect; default false',
      'client_name' => 'site: machine name of the client that owns this site',
      'drush_aliases' => 'site: Comma-separated list of additional Drush aliases through which this site can be accessed.',
    );
  }

  function init() {
    // FIXME: this should not be necessary?
    // NB: should be fixed by parent::init();
    $this->setProperty('type', 'wpsite');
    $this->setProperty('context_type', 'wpsite');

    parent::init();

    // FIXME: This is necessary otherwise provision crashes,
    // but I have no idea why it is necessary.
    $this->is_oid('wpplatform');
    $this->is_oid('platform');
  }

  function init_wpsite() {
    $this->setProperty('uri');

    // FIXME Should not be necessary?
    $this->setProperty('db_server');
    $this->setProperty('db_service_type');

    // we need to set the alias root to the platform root, otherwise drush will cause problems.
    $this->setProperty('wpplatform');

    $this->root = d($this->wpplatform)->root;

    // Required for provision's http/Provision/Service/http/public.php grant_server_list()
    // Avoids needing this kind of patch:
    // https://github.com/mlutfy/hosting_wordpress/commit/5713437a53745a92d0e1003cb5c0f6d8d0934b02#diff-60ccc9b0517a92298f464216ee226795
    $this->setProperty('platform', $this->wpplatform->name);

    // set this because this path is accessed a lot in the code, especially in config files.
    $this->site_path = $this->root . '/sites/' . $this->uri;

    $this->setProperty('site_enabled', true);
    $this->setProperty('language', 'en');
    $this->setProperty('client_name');
    $this->setProperty('aliases', array(), TRUE);
    $this->setProperty('redirection', FALSE);
    $this->setProperty('cron_key', '');
    $this->setProperty('drush_aliases', array(), TRUE);

    // this can potentially be handled by a Drupal sub class
    $this->setProperty('profile', 'default');

    // required for the way we use wp-cli (include)
    // otherwise the require_maybe() statements will fail.
    global $wp_version;
  }

  /**
   * Write out this named context to an alias file.
   */
  function write_alias() {
    $config = new Provision_Config_Drushrc_Alias($this->name, $this->properties);
    $config->write();
    foreach ($this->drush_aliases as $drush_alias) {
      $config = new Provision_Config_Drushrc_Alias($drush_alias, $this->properties);
      $config->write();
    }
  }
}

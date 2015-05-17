Provision WordPress
===================

EXPERIMENTAL and broken, work in progress.

Installation
============

- Copy the provision_wordpress module in your ~/.drush/ directory.
- Copy and enable the hosting_wordpress module in your ~/hostmaster-7.x-3.x/sites/example.org/modules directory.
- Apply the 2 patches below (this will probably break install/verify of Drupal sites)
- Install wp-cli in ~/lib/wp-cli.phar and symlink it as ~/bin/wp (FIXME: use ~/.composer ?)

How to test
===========

- Create a new WP Platform (node/add/wpplatform)
- Create a new WP Site from there.
- NB: I have only tested on nginx so far. There may be rewrites missing for Apache.

TODO and known bugs
===================

* Can't run 'verify' on the platform (workaround: edit the platform node and re-save).
* Site 'verify' loses the database credentials in the vhost config.

How it works
============

- wpplatform verify creates a platform/example/wp-config.php file
  based on wp-config-sample.php, but uses the vhost variables so that
  we can install multiple sites in the same platform.

- TODO: need to define WP_CONTENT_DIR in wp-config.php to separate content.

Support
=======

This module is not currently supported. Feel free to send patches using the issue queue.

Copyright and license
=====================

(C) 2015 Mathieu Lutfy <mathieu@symbiotic.coop>
License: GPL v3 or later. http://www.gnu.org/copyleft/gpl.html

---------------

Patches on core that need a better fix:

- provision/http/Provision/Service/http/public.php

```
  function grant_server_list() {
    return array(
      $this->server,
      $this->context->wpplatform->server, // [ML] PATCH WP
    );
  }
```

- /var/aegir/hostmaster-7.x-3.0-beta1/profiles/hostmaster/modules/aegir/hosting/hosting.module:

```
function hosting_context_node_types() {
  return array('site', 'platform', 'server', 'wpplatform', 'wpsite');
}
```

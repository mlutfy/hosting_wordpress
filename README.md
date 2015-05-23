Hosting WordPress
=================

EXPERIMENTAL work in progress. Use at your own risk.

This module is not officially supported. No security announcement will be
published if an issue is found. See 'support' for more information.

See: https://www.drupal.org/node/1044692

Installation
============

- Copy the hosting_wordpress module in your ~/hostmaster-7.x-3.x/sites/example.org/modules directory.
- Enable the hosting_wpplatform and hosting_wpsite modules (drush @hostmaster en ...).
- Run "drush @hostmaster provision-verify" so that Aegir does the magic include in ~/.drush/drushrc.php (c.f. do#2300537).
- Install wp-cli in ~/lib/wp-cli/ (FIXME: use ~/.composer ?)
- Symlink wp-cli.php as ~/bin/wp, ex: "ln -s /var/aegir/lib/wp-cli/bin/wp /var/aegir/bin/wp"
- You will probably want to add /var/aegir/bin/ in your $PATH, ex: export PATH=$PATH:/var/aegir/bin
- Apply the 3 patches below (this will probably break install/verify of Drupal sites)

How to test
===========

- Create a new platform on your filesystem, ex: mkdir /var/aegir/platforms/wordpress-4.2.2/; cd /var/aegir/platforms/wordpress-4.2.2/; wp core download
- Create a new WP Platform (node/add/wpplatform)
- Create a new WP Site from there.
- Access the URL of your new site.
- NB: I have only tested on nginx so far. There may be rewrites missing for Apache.
- You can also use wp-cli in a site context, using, for example: "drush @mysite.example.org wp user list"

TODO and known bugs
===================

* Define WP_CONTENT_DIR in wp-config.php to separate content.
* Can't run 'verify' on the platform (workaround: edit the platform node and re-save).
* Site 'verify' loses the database credentials in the vhost config.
* CiviCRM support? https://github.com/andy-walker/wp-cli-civicrm

How it works
============

- provision_wordpress and hosting_wordpress implement new entities for
  the 'wpplatform' and 'wpsite' (highly inspired from 'platform' and 'site').

- while it would be great to re-use the platform/site from Aegir, they make
  too many assumptions related to Drupal.

- wpplatform verify creates a platform/example/wp-config.php file
  based on wp-config-sample.php, but uses the vhost variables so that
  we can install multiple sites in the same platform.

Support
=======

This module is not currently supported. Feel free to send patches using the issue queue.

Copyright and license
=====================

Provision Wordpress  
(C) 2015 Mathieu Lutfy <mathieu@symbiotic.coop>

Distributed under the terms of the GNU Affero General public license v3 (AGPL).  
http://www.gnu.org/licenses/agpl.html

```
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
```

---------------

Patch on wp-cli:

./php/wp-settings-cli.php

- Utils\maybe_require( '3.5-alpha-22024', ABSPATH . WPINC . '/class-wp-embed.php' );
+ require( ABSPATH . WPINC . '/class-wp-embed.php' );

==> same for revision.php and post-formats.php. Only causes warnings, but annoying.

Patches on Aegir core that need a better fix:

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

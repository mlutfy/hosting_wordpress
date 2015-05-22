Provision WordPress
===================

EXPERIMENTAL work in progress. Use at your own risk.

See: https://www.drupal.org/node/1044692

Also see the hosting_wordpress module for the front-end:  
https://github.com/mlutfy/hosting_wordpress

Installation
============

- Copy and enable the hosting_wordpress module in your ~/hostmaster-7.x-3.x/sites/example.org/modules directory.
- Apply the 3 patches included in the patches directory (this will probably break install/verify of Drupal sites)
- Install wp-cli in ~/lib/wp-cli.phar and symlink it as ~/bin/wp (FIXME: use ~/.composer ?)

How to test
===========

- Create a new platform on your filesystem, ex: mkdir /var/aegir/platforms/wordpress-4.2.2/; cd /var/aegir/platforms/wordpress-4.2.2/; wp core download
- Create a new WP Platform (node/add/wpplatform)
- Create a new WP Site from there.
- Access the URL of your new site.
- NB: I have only tested on nginx so far. There may be rewrites missing for Apache.

TODO and known bugs
===================

* Define WP_CONTENT_DIR in wp-config.php to separate content.
* Fix having to restart nginx manually after install.
* Can't run 'verify' on the platform (workaround: edit the platform node and re-save).
* Site 'verify' loses the database credentials in the vhost config.
* CiviCRM support? https://github.com/andy-walker/wp-cli-civicrm
* Better wp-cli alias integration?

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


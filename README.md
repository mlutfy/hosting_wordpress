EXPERIMENTAL and non-working. For fun & educational purposes only.

Also see the hosting_wordpress module for the front-end.

License: GPL v2 or later.

mathieu@symbiotic.coop

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

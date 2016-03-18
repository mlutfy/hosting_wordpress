<?php

/**
 * A handler for the Platform release field.
 *
 * @ingroup views_field_handlers
 */
class hosting_platform_handler_field_release extends views_handler_field {
  function render($values) {
    $node = node_load($values->nid);
    if (empty($node->release)) {
      return "";
    }
    return l("{$node->release->title} {$node->release->version}", "node/{$node->release->nid}");
  }
}

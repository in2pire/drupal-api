<?php

/**
 * @file
 * IN2PIRE Core API.
 */

/**
 * Creates a web-accessible URL for a stream to an external or local file.
 *
 * Compatibility: normal paths and stream wrappers.
 *
 * There are two kinds of local files:
 * - "managed files", i.e. those stored by a Drupal-compatible stream wrapper.
 *   These are files that have either been uploaded by users or were generated
 *   automatically (for example through CSS aggregation).
 * - "shipped files", i.e. those outside of the files directory, which ship as
 *   part of Drupal core or contributed modules or themes.
 *
 * @param $uri
 *   The URI to a file for which we need an external URL, or the path to a
 *   shipped file.
 *
 * @return
 *   A string containing a URL that may be used to access the file.
 *   If the provided string already contains a preceding 'http', 'https', or
 *   '/', nothing is done and the same string is returned. If a stream wrapper
 *   could not be found to generate an external URL, then FALSE is returned.
 *
 * @see file_create_url()
 */
function furl($uri) {
  return file_create_url($uri);
}

/**
 * Try to dump variables in different ways.
 */
function k_dump() {
  $args = func_get_args();

  if(function_exists('has_krumo')) {
    has_krumo();
    return call_user_func_array('krumo', $args);
  }

  if (ini_get('html_errors') && extension_loaded('xdebug')) {
    return call_user_func_array('var_dump', $args);
  }

  echo '<pre>';

  foreach($args as $arg) {
    print_r($arg);
  }

  echo '</pre>';
}

/**
 * Dump variables using drupal message.
 */
function m_dump() {
  $args = func_get_args();

  foreach($args as $arg) {
    drupal_set_message('<pre>' . print_r($args, 1) . '</pre>');
  }
}

/**
 * Dump SelectQuery or raw query with arguments.
 * @param  $query A SelectQuery object or raw query string.
 * @param  $args  An array og arguments.
 */
function q_dump($query, $args = array()) {
  if (is_object($query)) {
    $args = $query->getArguments();
    $raw  = $query->__toString();
  }
  else {
    $raw = $query;
  }

  if (!empty($args)) {
    krsort($args);
  }

  $raw = str_replace(array('{', '}'), '', $raw);

  foreach ($args as $arg => $value) {
    if (is_array($value)) {
      $raw = str_replace($arg, '"' . implode('", "', $value) . '"', $raw);
    }
    else {
      $raw = str_replace($arg, '"' . $value . '"', $raw);
    }
  }

  return $raw;
}

/**
 * Fix partial object.
 * @param  $object Incomplete object.
 * @return object Completed object.
 */
function fix_object(&$object) {
  if (!is_object ($object) && gettype($object) == 'object') {
    return ($object = unserialize(serialize($object)));
  }

  return $object;
}

/**
 * @see views_trim_text()
 */
function _trim_text($alter, $value) {
  if (!isset($alter['html'])) {
    $alter['html'] = true;
  }

  if (drupal_strlen($value) > $alter['max_length']) {
    $value = drupal_substr($value, 0, $alter['max_length']);

    // TODO: replace this with cleanstring of ctools
    if (!empty($alter['word_boundary'])) {
      $regex = "(.*)\b.+";

      if (function_exists('mb_ereg')) {
        mb_regex_encoding('UTF-8');
        $found = mb_ereg($regex, $value, $matches);
      }
      else {
        $found = preg_match("/$regex/us", $value, $matches);
      }

      if ($found) {
        $value = $matches[1];
      }
    }

    // Remove scraps of HTML entities from the end of a strings
    $value = rtrim(preg_replace('/(?:<(?!.+>)|&(?!.+;)).*$/us', '', $value));

    if (!empty($alter['ellipsis'])) {
      $value .= t('...');
    }
  }

  if (!empty($alter['html'])) {
    $value = _filter_htmlcorrector($value);
  }

  return $value;
}

/**
 * Shorten a string.
 * @param  string  $text
 *   String to be shortened.
 * @param  number  $max_length
 *   Maximum lenght of the string, the rest gets truncated.
 * @param  boolean $word_boundary
 *   Trim only on a word boundary.
 * @param  boolean $ellipsis
 *   Show an ellipsis (...) at the end of the trimmed string.
 * @param  boolean $html
 *   Take sure that the html is correct.
 * @return string
 *   Shortened string.
 */
function shorten_text($text, $max_length, $word_boundary = TRUE, $ellipsis = TRUE, $html = TRUE) {
  return _trim_text(array(
    'html'          => $html,
    'max_length'    => $max_length,
    'word_boundary' => $word_boundary,
    'ellipsis'      => $ellipsis,
  ), $text);
}

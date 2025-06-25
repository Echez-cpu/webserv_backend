<?php
    $file = dirname(dirname(__FILE__)) . '/d_struct/data.json';
    $dataJson = file_get_contents($file);

    $data = json_decode($dataJson, true);

    $entries = $data['entries'];

    
    usort($entries, function($a, $b) {
        return $a['id'] - $b['id'];
    });

    foreach ($entries as $entry) {
        echo "<tr>";
        echo "<td>" . $entry['id'] . "</td>";
        echo "<td>" . $entry['description'] . "</td>";
        echo "<td>" . $entry['price'] . "</td>";
        echo "<td>" . $entry['phone'] . "</td>";
        echo "<td> <img src='images/{$entry['id']}.jpeg' width='50' height='50' alt='car image'/>  </td>";
        echo "</tr>";
    }
?>

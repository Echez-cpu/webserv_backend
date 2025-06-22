<?php
    $file = dirname(dirname(__FILE__)) . '/data/data.json';
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
        echo "<td> <img src='cars/{$entry['id']}.jpeg' width='100' height='100' alt='board picture'/>  </td>"; // images == cars..
        echo "</tr>";
    }
?>
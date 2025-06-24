<?php
$file = dirname(dirname(__FILE__)) . '/d_struct/data.json';
$dataJson = file_get_contents($file);

$data = json_decode($dataJson, true);


$post_data_file = $argv[1];
$post_data = file_get_contents($post_data_file);
parse_str($post_data, $_POST);

$entries = $data['entries'];

$description = $_POST['description'];
$price = $_POST['price'];
$phone = $_POST['phone'];

$usedIds = array_column($entries, 'id');
sort($usedIds);
for ($i = 1; $i <= count($entries) + 1; $i++) {
    if (!in_array($i, $usedIds)) {
        $id = $i;
        break;
    }
}

echo $id;

$entries[] = array(
  'id' => (string)$id,
  'description' => $description,
  'price' => $price,
  'phone' => $phone,
);

$data['entries'] = $entries;

$dataJson = json_encode($data);

file_put_contents($file , $dataJson);

?>
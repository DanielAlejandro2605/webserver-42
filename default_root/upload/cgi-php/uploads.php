<?php

$htmlString = <<<HTML
<!DOCTYPE html>
<html lang='en'>
   <head>
      <meta charset='UTF-8'>
      <link type='image/png' rel='icon' href='./skull.png'>
      <title>CGI Page</title>
      <style>
         body {
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f5f5f5;
            margin: 0;
            padding: 50px;
         }
         h1 {
            color: #333333;
         }
         p {
            color: #666666;
         }
         #background-video {
            visibility : hidden;
         }
      </style>
      <link rel='stylesheet' href='./webserv.css'>
   </head>
   <body>
HTML;

if (isset($_SERVER['REQUEST_METHOD']) && $_SERVER['REQUEST_METHOD'] === 'GET')
{
    $htmlString .= "<h1>This script is better with a POST METHOD 🍭 </h1>";
}

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (isset($_FILES['file'])) {
        $file_error = $_FILES['file']['error']; // debug
        $file_name = $_FILES['file']['name'];
        // $upload_file = $file_name;
        $upload_file = urldecode($file_name);
        $upload_file = str_replace(' ', '_', $file_name);
        // $upload_file = preg_replace('/\s+/', '-', $file_name);

        if ($file_error === UPLOAD_ERR_OK) {
            if (move_uploaded_file($_FILES['file']['tmp_name'], $upload_file)) {
                echo "200\r\n";
                $htmlString .= "      <h1>The file <a href='./{$upload_file}'>{$file_name}</a> has been uploaded successfully using PHP-CGI!</h1>\n";
            } else {
                echo "400\r\n";
                $htmlString .= "      <p>Error uploading the file {$file_name}.</p>\n";
            }
        } else {
            echo "500\r\n";
            $htmlString .= "      <p>File upload error: {$file_error}</p>\n";
        }
    } else {
        if (!isset($_POST['name']) || !isset($_POST['email']) || !isset($_POST['message'])
            || empty($_POST['name']) || empty($_POST['email']) || empty($_POST['message']))
        {
            echo "400\r\n";
            $htmlString .= "<h1>🤔 Hmmm ... Seems like it's not a form whoever sent this!</h1>";
            $htmlString .= "<p>Look what I recovered:</p>";
            $data = file_get_contents("php://input");

            $htmlString .= "<p>$data</p>";
        } else {
            echo "200\r\n";
            $name = htmlspecialchars($_POST["name"]);
            $email = htmlspecialchars($_POST["email"]);
            $message = htmlspecialchars($_POST["message"]);
    
            $htmlString .= "<h2>Form Submission Result</h2>";
            $htmlString .= "<p>Name: $name</p>";
            $htmlString .= "<p>Email: $email</p>";
            $htmlString .= "<p>Message: $message</p>";
        }
    }
}

$htmlString .= "      <p> Coded by mflores- and dnieto-c </p>\n";
$htmlString .= "      <video id='background-video' autoplay loop muted>\n";
$htmlString .= "         <source src='./webserv.mp4' type='video/mp4'>\n";
$htmlString .= "      </video>\n";
$htmlString .= "   </body>\n";
$htmlString .= "</html>\n";
echo $htmlString;

?>

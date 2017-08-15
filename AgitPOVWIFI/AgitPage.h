// Ajoute accept-charset="UTF-8" au formulaire pour les accents
///// Use 'post' from a webpage to send a word from https://gist.github.com/bbx10/5a2885a700f30af75fc5


///// page that will be sent over to the client
const char INDEX_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
"<title>AgitPOV</title>"
"<style>"
"body {  font-family: Arial, Helvetica, sans-serif;Color: #111111;font-size: 30px; margin-left:auto;margin-right:auto;width:400px; }"
"</style>"
"</head>"
"<body bgcolor=\"#b72e1c\" >"
"<div id=\"page\">"
"<FORM action=\"/\" accept-charset=\"UTF-8\" method=\"post\">"
"<P>"
"<br>"
"<INPUT type=\"text\" name=\"AgitPOV\" value=\"\" placeholder=\"VOTRE MOT\" maxlength=\"26\" style=\"font-size:30px ; background-color:#D92e1c ; border: none;\"> <BR>"
"<input type=\"radio\" name=\"color\" value=\"0\"> rouge <BR>"
"<input type=\"radio\" name=\"color\" value=\"1\"> orange <BR>"
"<input type=\"radio\" name=\"color\" value=\"2\"> jaune<BR>"
"<input type=\"radio\" name=\"color\" value=\"3\"> vert<BR>"
"<input type=\"radio\" name=\"color\" value=\"4\"> bleu<BR>"
"<input type=\"radio\" name=\"color\" value=\"5\"> mauve<BR>"
"<input type=\"radio\" name=\"color\" value=\"6\"> lumiere<BR>"
"<input type=\"radio\" name=\"color\" value=\"7\"> arc-en-ciel<BR>"

"<INPUT type=\"submit\" value=\"Envoyez\" style=\"font-size:26px ; background-color:#DD622D ; border: none;\">"
"</div>"
"</P>"
"</FORM>"
"</body>"
"</html>";

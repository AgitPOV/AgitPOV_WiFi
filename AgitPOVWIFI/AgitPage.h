// Ajoute accept-charset="UTF-8" au formulaire pour les accents
///// Use 'post' from a webpage to send a word from https://gist.github.com/bbx10/5a2885a700f30af75fc5


///// page that will be sent over to the client
char INDEX_HTML[] =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<meta charset=\"UTF-8\">"
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
"<INPUT type=\"text\" name=\"AgitPOV\" value=\"";
char INDEX_HTML2[]=
"\" placeholder=\"VOTRE MOT\" maxlength=\"26\" style=\"font-size:30px ; background-color:#D92e1c ; border: none; text-transform:uppercase;\"> <BR>"
 "&nbsp;<input type=\"radio\" name=\"color\" value=\"0\"> rouge <BR>"
 "&nbsp;<input type=\"radio\" name=\"color\" value=\"1\"> orange <BR>"
 "&nbsp;<input type=\"radio\" name=\"color\" value=\"2\"> jaune<BR>"
 "&nbsp;<input type=\"radio\" name=\"color\" value=\"3\"> vert<BR>"
 "&nbsp;<input type=\"radio\" name=\"color\" value=\"4\"> bleu<BR>"
 "&nbsp;<input type=\"radio\" name=\"color\" value=\"5\"> mauve<BR>"
 "&nbsp;<input type=\"radio\" name=\"color\" value=\"6\"> blanc<BR>"
 "&nbsp;<input type=\"radio\" name=\"color\" value=\"42\"> arc-en-ciel<BR>"
 "&nbsp;<input type=\"checkbox\" name=\"dark\" value=\"1\"> mode sombre<BR>"
 "&nbsp;<INPUT type=\"submit\" name=\"cmd\" value=\"GO\" style=\"font-size:26px ; background-color:#DD622D ; border: none;\">"

#ifdef SPIFFS_DEBUGGING
"<INPUT type=\"submit\" name=\"cmd\" value=\"LS\">"
"<INPUT type=\"submit\" name=\"cmd\" value=\"AFFICHE\">"
"<INPUT type=\"submit\" name=\"cmd\" value=\"EFFACE\">"
#endif

"</div>"
"</P>"
"</FORM>"
"</body>"
"</html>";

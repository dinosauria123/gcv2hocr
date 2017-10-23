#!/bin/bash
#cd ~
echo '{"requests":[{"image":{"content":"' > ./temp.json
openssl base64 -in $1 | cat >> ./temp.json
echo '"},
"features":{"type":"TEXT_DETECTION","maxResults":2048},"imageContext":{"languageHints":"ja"}}]}' >> ./temp.json
curl -k -s -H "Content-Type: application/json" https://vision.googleapis.com/v1/images:annotate?key=$2 --data-binary @./temp.json > "$1.json"
rm ./temp.json

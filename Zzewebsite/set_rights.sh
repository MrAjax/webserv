#!/bin/sh

# 4 : Lecture (r)
# 2 : Écriture (w)
# 1 : Exécution (x)

GET="chmod 400"
GET_POST="chmod 600"
POST="chmod 200"
POST_DELETE="chmod 700"
DELETE="chmod 700"
NONE="chmod 000"

# admin
$(POST_DELETE) admin/files
$(GET) admin/login
$(GET) admin/welcome

# data
$(POST) data

# error_page
$(GET) error_page

# ground beetle
$(GET) ground_beetle

# ground beetle form
$(GET_POST) ground_beetle_form/ground_beetle_form.html
$(GET_POST) ground_beetle_form/confirmation.html
$(GET_POST) ground_beetle_form/beetlemania.jpg

# index
$(GET) index

# shared_files
$(GET_POST) shared_files

# user
$(POST_DELETE) user/files
$(GET) user/login
$(GET) user/welcome

# /
$(GET) favicon.ico
$(GET) landscape.jpeg
$(NONE) set_rights.sh
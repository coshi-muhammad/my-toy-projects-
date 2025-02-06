from flask import Flask,request,send_from_directory
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__,static_folder="static")



@app.route('/')
def homeScreen():
    return send_from_directory("template","index.html")

@app.route('/images/<filename>')
def getImages(filename):
    return send_from_directory("images",filename)


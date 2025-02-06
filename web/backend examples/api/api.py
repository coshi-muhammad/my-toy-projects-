from flask import Flask
from flask import jsonify
from flask import request
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)
app.config["SQLALCHEMY_DATABASE_URI"] = 'sqlite:///data.db'
db = SQLAlchemy(app)

class Drink(db.Model):
    id = db.Column(db.Integer,primary_key = True)
    name = db.Column(db.String(80),unique = True,nullable = False)
    description = db.Column(db.String(120),)
    
    def __repr__(self):
        return f"{self.name} - {self.description}"


@app.route('/')
def index():
    return "hello world!"

@app.route('/drinks')
def getDrinks():
    drinks = Drink.query.all()
    out_put = []
    for drink in drinks:
        drink_data = {'name':drink.name,'description':drink.description}
        out_put.append(drink_data)
    return {'drinks':out_put}


@app.route('/drinks/<id>')
def getDrinkById(id):
    drink = Drink.query.get_or_404(id)
    return {'name':drink.name,'description':drink.description}

@app.route('/drinks',methods=['POST'])
def addDrink():
    drink = Drink(name=request.json['name'],description = request.json['description'])
    db.session.add(drink)
    db.session.commit()
    return {'id':drink.id}

@app.route('/drinks/<id>',methods=['DELETE'])
def deleteDrink(id):
    drink = Drink.query.get(id)
    db.session.delete(drink)
    db.session.commit()
    return "the drink is ass"

@app.route('/drinks',methods=['PUT'])
def modifyDrink():
    drink = Drink(name=request.json['name'],description= request.json['description'],id=request.json['id'])
    db.session.merge(drink)
    db.session.commit()
    return "i hope this isn't a new coke moment"

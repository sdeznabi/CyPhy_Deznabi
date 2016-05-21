from flask import g

from wtforms.validators import Email

from server import db, flask_bcrypt

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    email = db.Column(db.String(120), unique=True, nullable=False, info={'validators': Email()})
    password = db.Column(db.String(80), nullable=False)
    sensors = db.relationship('Sensor', backref='user', lazy='dynamic')

    def __init__(self, email, password):
        self.email = email
        self.password = flask_bcrypt.generate_password_hash(password)

    def __repr__(self):
        return '<User %r>' % self.email

class Sensor(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(120), nullable=False)
    description = db.Column(db.Text, nullable=False)
    user_id = db.Column(db.Integer, db.ForeignKey('user.id'))
    created_at = db.Column(db.DateTime, default=db.func.now())
    readings = db.relationship('Reading', backref='sensor', lazy='dynamic')

    def __init__(self, name, description):
        self.name = name
        self.description = description
        self.user_id = g.user.id

    def __repr__(self):
        return '<Post %r>' % self.name

class Reading(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    value = db.Column(db.Float, nullable=False)
    sensor_id = db.Column(db.Integer, db.ForeignKey('sensor.id'))
    created_at = db.Column(db.DateTime, default=db.func.now())

    def __init__(self, value, sensor_id):
        self.value = value
        self.sensor_id = sensor_id

    def __repr__(self):
        return '<Reading %r>' % self.id

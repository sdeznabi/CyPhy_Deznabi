from flask import g, request
from flask.ext import restful
from flask.ext.restful import reqparse

from datetime import datetime, timedelta
from server import api, db, flask_bcrypt, auth
from models import User, Sensor, Reading
from forms import UserCreateForm, SessionCreateForm, SensorCreateForm, ReadingCreateForm
from serializers import UserSerializer, SensorSerializer, ReadingSerializer

@auth.verify_password
def verify_password(email, password):
    user = User.query.filter_by(email=email).first()
    if not user:
        return False
    g.user = user
    return flask_bcrypt.check_password_hash(user.password, password)

class UserView(restful.Resource):
    def post(self):
        form = UserCreateForm()
        if not form.validate_on_submit():
            return form.errors, 422

        user = User(form.email.data, form.password.data)
        db.session.add(user)
        db.session.commit()
        return UserSerializer(user).data

class SessionView(restful.Resource):
    def post(self):
        form = SessionCreateForm()
        if not form.validate_on_submit():
            return form.errors, 422

        user = User.query.filter_by(email=form.email.data).first()
        if user and flask_bcrypt.check_password_hash(user.password, form.password.data):
            return UserSerializer(user).data, 201
        return '', 401

class SensorListView(restful.Resource):
    @auth.login_required
    def get(self):
        sensors = Sensor.query.filter_by(user_id=g.user.id).all()
        return SensorSerializer(sensors, many=True).data

    @auth.login_required
    def post(self):
        form = SensorCreateForm()
        if not form.validate_on_submit():
            return form.errors, 422
        sensor = Sensor(form.name.data, form.description.data)
        db.session.add(sensor)
        db.session.commit()
        return SensorSerializer(sensor).data, 201

class SensorView(restful.Resource):
    @auth.login_required
    def get(self, id):
        sensors = Sensor.query.filter_by(id=id, user_id=g.user.id).first()
        return SensorSerializer(sensors).data

#class ReadingListView(restful.Resource):
#    @auth.login_required
#    def post(self):
#        form = ReadingCreateForm()
#        if not form.validate_on_submit():
#            return form.errors, 422
#        reading = Reading(form.value.data, form.sensor_id.data)
#        db.session.add(reading)
#        db.session.commit()
#        return ReadingSerializer(reading).data, 201

class ReadingListView(restful.Resource):
    #@auth.login_required
    def post(self):
        parser = reqparse.RequestParser()
	print request.headers
	#parser.add_argument('X-Temperature', required=True, location='headers', help="Temperature can't be blank!")
        parser.add_argument('Content-Type', required=True, location='headers', help="Temperature can't be blank!")
        #parser.add_argument('X-Sensor-Id', required=True, location='headers', help="Sensor-Id can't be blank!")
        args = parser.parse_args()
	print args
        idx = args['Content-Type'].find('Temperature: ')
        temperature = args['Content-Type'][idx+13:idx+15]
        idx = args['Content-Type'].find('Sensor-Id: ')
        sensor_id = args['Content-Type'][idx+11]
        
        print "TEMP:", temperature
        print "ID:", sensor_id
        reading = Reading(temperature, sensor_id)
        db.session.add(reading)
        db.session.commit()
        return ReadingSerializer(reading).data, 201


api.add_resource(UserView, '/api/v1/users')
api.add_resource(SessionView, '/api/v1/sessions')
api.add_resource(SensorListView, '/api/v1/sensors')
api.add_resource(SensorView, '/api/v1/sensors/<int:id>')
api.add_resource(ReadingListView, '/api/v1/readings')


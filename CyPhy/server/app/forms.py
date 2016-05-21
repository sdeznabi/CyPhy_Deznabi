from flask.ext.wtf import Form

from wtforms_alchemy import model_form_factory
from wtforms import StringField, IntegerField
from wtforms.validators import DataRequired

from app.server import db
from models import User, Sensor, Reading

BaseModelForm = model_form_factory(Form)

class ModelForm(BaseModelForm):
    @classmethod
    def get_session(self):
        return db.session

class UserCreateForm(ModelForm):
    class Meta:
        model = User

class SessionCreateForm(Form):
    email = StringField('name', validators=[DataRequired()])
    password = StringField('password', validators=[DataRequired()])

class SensorCreateForm(ModelForm):
    class Meta:
        model = Sensor

class ReadingCreateForm(ModelForm):
    sensor_id = IntegerField('sensor_id', validators=[DataRequired()])
    class Meta:
        model = Reading

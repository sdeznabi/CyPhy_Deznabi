from marshmallow import Serializer, fields

class UserSerializer(Serializer):
    class Meta:
        fields = ("id", "email")

class ReadingSerializer(Serializer):
    created_at = fields.DateTime(format="%s")
    class Meta:
        fields = ("id", "value", "created_at")

class SensorSerializer(Serializer):
    user = fields.Nested(UserSerializer)
    values = fields.Nested(ReadingSerializer, many=True, attribute="readings")
    created_at = fields.DateTime()
    key = fields.String(attribute="name")
    class Meta:
        fields = ("id", "key", "description", "user", "values", "created_at")

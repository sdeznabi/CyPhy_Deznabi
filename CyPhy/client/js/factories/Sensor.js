Monitfy.factory('Sensor', function(Restangular) {
    var Sensor;
    Sensor = {
        get: function() {
            return Restangular
            .one('sensors')
                .getList();
        },
        create: function(data) {
            return Restangular
            .one('sensors')
                .customPOST(data);
        }
    };
    return Sensor;
})

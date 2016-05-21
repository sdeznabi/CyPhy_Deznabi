Monitfy.controller('SensorCreateCtrl', function($scope, $location, Sensor) {
    $scope.submit = function(isValid, sensor) {
        $scope.submitted = true;
        $scope.sensorCreateForm.$setDirty();

        if (!isValid) {
            return;
        }

        Sensor.create(sensor).then(function(response) {
            $location.path('/');
        });
    };
})

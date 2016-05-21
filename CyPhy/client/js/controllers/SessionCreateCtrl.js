Monitfy.controller('SessionCreateCtrl', function($scope, $location, Session, AuthService) {
    $scope.submit = function(isValid, credentials) {
        $scope.submitted = true;
        $scope.authenticationForm.$setDirty();

        if (!isValid) {
            return;
        }

        AuthService.login(credentials).then(function(user) {
            $location.path('/sensors/list')
        }, function(response) {
            $scope.failedLoginAttempt = true;
        });
    };
})

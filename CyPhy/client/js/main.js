window.Monitfy = angular.module('Monitfy', ['ngRoute', 'restangular', 'LocalStorageModule','nvd3'])

.run(function($location, Restangular, AuthService) {
    Restangular.setFullRequestInterceptor(function(element, operation, route, url, headers, params, httpConfig) {
        if (AuthService.isAuthenticated()) {
            headers['Authorization'] = 'Basic ' + AuthService.getToken();
        }
        return {
            headers: headers
        };
    });

    Restangular.setErrorInterceptor(function(response, deferred, responseHandler) {
        if (response.config.bypassErrorInterceptor) {
            return true;
        } else {
            switch (response.status) {
                case 401:
                    AuthService.logout();
                    $location.path('/sessions/create');
                    break;
                default:
                    throw new Error('No handler for status code ' + response.status);
            }
            return false;
        }
    });
})

.config(function($routeProvider, RestangularProvider) {

    RestangularProvider.setBaseUrl('http://ec2-52-18-212-113.eu-west-1.compute.amazonaws.com:5000/api/v1');

    var partialsDir = '../partials';

    var redirectIfAuthenticated = function(route) {
        return function($location, $q, AuthService) {

            var deferred = $q.defer();

            if (AuthService.isAuthenticated()) {
                deferred.reject()
                $location.path(route);
            } else {
                deferred.resolve()
            }

            return deferred.promise;
        }
    }

    var redirectIfNotAuthenticated = function(route) {
        return function($location, $q, AuthService) {

            var deferred = $q.defer();

            if (! AuthService.isAuthenticated()) {
                deferred.reject()
                $location.path(route);
            } else {
                deferred.resolve()
            }

            return deferred.promise;
        }
    }

    $routeProvider
        .when('/', {
            controller: 'HomeDetailCtrl',
            templateUrl: partialsDir + '/home/detail.html',
            //templateUrl: partialsDir + '/sensor/list.html',
            resolve: {
                //redirectIfNotAuthenticated: redirectIfNotAuthenticated('/sessions/create')
                redirectIfAuthenticated: redirectIfAuthenticated('/sensors/list')
            }

        })
        .when('/sessions/create', {
            controller: 'SessionCreateCtrl',
            templateUrl: partialsDir + '/session/create.html',
            resolve: {
                redirectIfAuthenticated: redirectIfAuthenticated('/sensors/list')
            }
        })
        .when('/sessions/destroy', {
            controller: 'SessionDestroyCtrl',
            templateUrl: partialsDir + '/session/destroy.html'
        })
        .when('/users/create', {
            controller: 'UserCreateCtrl',
            templateUrl: partialsDir + '/user/create.html'
        })
        .when('/sensors/list', {
            controller: 'SensorListCtrl',
            templateUrl: partialsDir + '/sensor/list.html',
            resolve: {
                redirectIfNotAuthenticated: redirectIfNotAuthenticated('/sessions/create')
            }
        })
        .when('/sensors/create', {
            controller: 'SensorCreateCtrl',
            templateUrl: partialsDir + '/sensor/create.html',
            resolve: {
                redirectIfNotAuthenticated: redirectIfNotAuthenticated('/sessions/create')
            }
        });
})

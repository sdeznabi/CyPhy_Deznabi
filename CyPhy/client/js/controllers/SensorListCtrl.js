Monitfy.controller('SensorListCtrl', function($scope, Sensor) {
    $scope.getSensorList = function(){ Sensor.get().then(function(sensors) {
        $scope.sensors = sensors;
         /* Chart options*/
        $scope.options = {
            chart: {
                type: 'lineChart',
                height: 450,
                margin : {
                    top: 20,
                    right: 20,
                    bottom: 40,
                    left: 55
                },
                x: function(d){ return d.created_at; },
                y: function(d){ return d.value; },
                useInteractiveGuideline: true,
                dispatch: {
                    stateChange: function(e){ console.log("stateChange"); },
                    changeState: function(e){ console.log("changeState"); },
                    tooltipShow: function(e){ console.log("tooltipShow"); },
                    tooltipHide: function(e){ console.log("tooltipHide"); }
                },
                xAxis: {
                    axisLabel: 'Time',
                    tickFormat: function(d) {
                        return d3.time.format('%y/%m/%d,%H:%M')(new Date(d*1000))
                    },
                },
                yAxis: {
                    axisLabel: 'Temperature (c)',
                    tickFormat: function(d){
                        return d3.format('.02f')(d);
                    },
                    axisLabelDistance: -10
                },
                callback: function(chart){
                    //console.log("!!! lineChart callback !!!");
                }
            },
            title: {
                enable: true,
                text: 'Sensor Temperature'
            },
            subtitle: {
                enable: true,
                text: 'Subtitle',
                css: {
                    'text-align': 'center',
                    'margin': '10px 13px 0px 7px'
                }
            },
            caption: {
                enable: true,
                html: '<b></b>',
                css: {
                    'text-align': 'justify',
                    'margin': '10px 13px 0px 7px'
                }
            }
        };

        /*chart data*/
        $scope.data = sensors;
        //$scope.api.refresh();
        //console.log("refreshed");

    });
    }

    setInterval(function() {
        $scope.$apply(function() {
            $scope.getSensorList();
            //$scope.api.refresh();
            //console.log("refreshed");
        })
    }, 5*1000);

    $scope.getSensorList();
})

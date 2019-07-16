-- author: Komkrit Thongbunchu
-- contact: earthkmutt59@gmail.com

-- Function Subscriber node from ROS for drive motor
function motor_cb(msg)
    data = msg.data
    for i=1,2,1 do
        motorSignal[i] = data[i]
    end
    sim.setJointTargetVelocity(rightJoint, motorSignal[1])
    sim.setJointTargetVelocity(leftJoint, motorSignal[2])

end

-- Function Subscriber node from ROS for plot graph 
function graph_cb(msg)
    data = msg.data
    sim.setGraphUserData(graph,"Left_Wheel",data[1])
    sim.setGraphUserData(graph,"Right_Wheel",data[2])
end

function sysCall_init() 

    -- Define component of robot from V-REP simulation such as sensors , motors , etc.
    objHandle=sim.getObjectAssociatedWithScript(sim.handle_self)
    infrared1=sim.getObjectHandle("dr20_infraredSensor1_")
    infrared6=sim.getObjectHandle("dr20_infraredSensor6_")
    ultrasonic=sim.getObjectHandle("dr20_ultrasonicSensor_")
    leftJoint=sim.getObjectHandle("dr20_leftWheelJoint_")
    rightJoint=sim.getObjectHandle("dr20_rightWheelJoint_")
    ultrasonicJoint=sim.getObjectHandle("dr20_ultrasonicSensorJoint_")

    -- Define graph from V-REP simulation
    graph = sim.getObjectHandle('Signal')

    -- Define initial variables
    motorSignal = {0.0,0.0}

    -- Check if the required ROS plugin is loaded
    moduleName=0
    moduleVersion=0
    index=0
    pluginNotFound=true
    while moduleName do
        moduleName,moduleVersion=sim.getModuleName(index)
        if (moduleName=='RosInterface') then
            pluginNotFound=false
        end
        index=index+1
    end
    if (pluginNotFound) then
        sim.displayDialog('Error','The RosInterface was not found.',sim.dlgstyle_ok,false,nil,{0.8,0,0,0,0,0},{0.5,0,0,1,1,1})
    end

    -- If found then start the subscribers and publishers
    if (not pluginNotFound) then
        local OutputdriveName='/wheelsnode/Outputdrive'
        local PlotSignalName='/wheelsnode/SignalPlot'
        local infraRName = '/wheelsnode/inputinfraTopic'  

        -- Create the subscribers
        OutputdriveSub=simROS.subscribe(OutputdriveName,'std_msgs/Float32MultiArray','motor_cb')
        PlotSignalSub=simROS.subscribe(PlotSignalName,'std_msgs/Float32MultiArray','graph_cb')
    
        -- Create the publishers
        infraR_Pub=simROS.advertise('/'..infraRName,'std_msgs/Float32MultiArray')

        -- Start the client application (c++ node)
        result=sim.launchExecutable(simGetStringParameter(sim_stringparam_scene_path) .. '/../../../projects/2wheels/catkin_ws/src/2wheels_controller/bin/2wheels_controller',
            OutputdriveName.." "..PlotSignalName.." "..infraRName,0)
        
        if (result==false) then
            sim.displayDialog('Error','External ROS-Node not found',sim.dlgstyle_ok,false,nil,{0.8,0,0,0,0,0},{0.5,0,0,1,1,1})
        end
    end

    --          For Read Initial Sensor     --

    --Read Sensor Proximity

    -- Analog signal show distance of obstacles
    -- resR and resL are digital, 0 is not found and 1 is found obstacles
    -- distR and distL are analog show distance between robot and obstacles

    resR,distR=sim.readProximitySensor(infrared1)   
    resL,distL=sim.readProximitySensor(infrared6)

    infra_array = {resR, resL}

    -- Publish infrared sensor 
    simROS.publish(infraR_Pub,{data=infra_array})

end

function sysCall_actuation() 
    -- This function will run repeatly
end 

function sysCall_sensing()

    -- Analog signal show distance of obstacles
    -- resR and resL are digital, 0 is not found and 1 is found obstacles
    -- distR and distL are analog show distance between robot and obstacles

    resR,distR=sim.readProximitySensor(infrared1)   
    resL,distL=sim.readProximitySensor(infrared6)

    infra_array = {resR, resL}

    -- Publish infrared sensor 
    simROS.publish(infraR_Pub,{data=infra_array})

end

function sysCall_cleanup() 
    simROS.shutdownSubscriber(OutputdriveSub)
    simROS.shutdownSubscriber(PlotSignalSub)
    simROS.shutdownPublisher(infraR_Pub)
end 

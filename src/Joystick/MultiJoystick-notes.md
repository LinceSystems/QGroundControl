TO DO BEFORE PR
- Investigate in Joystick.cc:

      // This appears 4 times when calibrating only gimbal "setFunctionAxis: invalid axis index "
      void Joystick::setFunctionAxis(AxisFunction_t function, int axis)

  JoystickConfigController.cc might have something related to it, line 645, in _writeCalibration.

- something to check if joysticks has only buttons, and if so set maincontrolenabled to false. Otherwise buttonsonly joysticks will get maincontrol enabled, 
  and as they are not calibrated we won't be able to enable them, but advanced tab will be missing so we won't have the chance to set it up anyway. It is a race condition

- fix JoystickIndicator ( only used for sub )

DONE 
- JoystickConfig.qml, figuring out something so when we select as active joystick one
  that is already secondary active or vice versa, we toggle summary view, so the 
  joystickconfigcontrollers are created properly
- When we connect and disconnect we need to figure out a way that they come to the good state. See logic
- Figure out a way to calibrate with no main controls. Factor out in joystick _handlAxis between the situation
  where we have main+gimbal, only main or only gimbal. Then we need to change calibrated to false whenever we 
  click enable gimbal or main control. This way the user can be extra aware of the controls configured.
  Add a skipFn to our state machine roll/pitch/yaw/thr, so we can skip them and only configure gimbal axis. 
- Disable skip button for main axis if main control is enabled.
- Setup messages of why gimbal control is not enabled, multiconfigok returning false
- Manage in HarrisAerialCorePlugin setting manually mainControl disabled for both joysticks, so for herelink 
  it starts with only buttons by default

LOGIC
- Primary and secondary joystick are dealt with the same in terms of priority.
- If no configuration in settings, first joystick in the detected list will be first, and second will be second
- Once primary and secondary are set, we can not swap them ( It doesn't make a difference from the control/priority 
  point of view). If only 2 joysticks, and both are saved in settings, the only way of changing secondary
  to primary is clicking the button "forget previous second joystick" in joystickConfigGeneral.qml
- If primary and secondary are swapped ( due to disconnection of primary ), when primary is connected back it will
  have the same settings/control, it will just appear as secondary.
- If secondary is disconnected, when it is connected back it will have the same settings as well.
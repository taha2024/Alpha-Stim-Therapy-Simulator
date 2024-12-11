 ## Tested Scenarios
 
 **1. On/Off Switch**
  - This has been tested and works on the device. Device has power button in top left corner that turns on/off the device.
  - Screen turns off and buttons on device are disabled. Therapy is stopped and recorded (if record is turned on) if it is ongoing when power button is pressed.
 
 **2. Continuous Check for Skin Contact. Therapy Paused/Ended When Skin Contact is Lost**
  - This has been tested and works on the device. Device has an on screen Contact On/ Contact Off label that changes with the skin contact change.
  - Skin contact is controlled via the admin area using a dropdown selector. Turn to true to start skin contact. False to remove skin contact.
  - If in a therapy session, session is paused when contact is lost for 5 seconds. If contact is not resumed, therapy ends (is recorded if selected). Otherwise, resumes.
 
 **3-5. Frequency, Time, and Waveform options**
  - These have been tested and works on the device.
  - User can select frequency, time or waveform while not in a therapy session. 
  - Press select button while on the Therapy Session screen and highlight will move to time menu choices.
  - Use up/down buttons to move selection highlight above a certain time.
  - Press select again to lock in choice. Time selected is updated in bold and large timer display is updated.
  - Highlight will move to waveform menu. Use up/arrows again to highlight different options. Use select to lock in the choice.
  - Highlight will move to frequency menu. Use up/arrows again to highlight different options. Use select to lock in the choice.
  - Can use the return button while looking through menus to go to the previous menu. If on time menu, it will exit option selection mode and remove highlight.
 
 **6. Large Timer Display. Treatment Starts on Skin Contact ON.**
   - This have been tested and works on the device.
   - Large timer is in center of device screen. It displays the therapy time as it countsdown.
   - For simulation purposes it counts 1 minute every second
   - When skin contact is changed to true in the admin area, therapy begins.
   - Timer On, Treating, Contact On labels switch on the screen.
   - When a therapy ends, skin contact is changed to false by default.
   
 **7. Current Control for Device**
   - This have been tested and works on the device.
   - Can be increased/decreased while the user is in a session.
   - User can use the up/down buttons while in treatment mode to raise level by 50 uA or lower by 100 uA
   - Minimum is 0 uA max is 500 uA
   - Power level 0 is 0 uA, power level 10 is 500 uA
   - By default each session starts off at 100 uA
    
 **8. 30 Minute Auto Off**
   - This have been tested and works on the device.
   - Inactivity is checked for when device is turned on.
   - It does not check for inactivity when the device is in treatment mode.
   - Counter in admin area shows the amount of inactive time
   - For simulation purposes it counts 1 minute every second
   - Inactive time can be increased for simulation purposes using the button beside the timer.
   - If user presses a button/screen tab on the device, the timer is reset.
   - When it hits 30:00 minutes. The device turns off.
     
 **9. Battery level indicator**
   - This have been tested and works on the device.
   - There is a battery level indicator in the upper left corner of the therapy screen.
   - It depletes at a defualt rate of 1% every 20 seconds while not in therapy
   - Depletes at default rate of 1% every 18 seconds while in therapy. If power level is increased/decreased, the burn rate is faster/slower.
   - Minimum burn rate is 1% every 20 seconds. Max burn rate is 1% every 10 seconds.
   - Battery can be manually depleted/restored for simulation purposes via the admin area selector.
   - When battery level reaches 5% a message appears on the screen warning the user.
   - When battery level reaches 2% a warning message appears again and the device screen and buttons power off. 
   - Device can't be turned on if at 2% battery or lower.
   - Battery only depletes when device is on.
   - If 2% warning happens when therapy is ongoing, the session is ended (and recorded if selected)
    
  **10. Recording**
   - This have been tested and works on the device.
   - User can press record button at any time while device is on to turn on recording.
   - Recording label will show on the screen. Not Recording label will show when it's not turned on.
   - When therapy ends, if record is set to on, therapy duration, waveform, frequency, powerlevel (1-10) and start time are recorded and added to the list of recorded therapies that can be seen on the Recorded Therapies screen. They are displayed newest to oldest.
   - If list is long, a scroll bar appears and the user can use the up/down buttons to scroll it.
  
  **11. Device Disable Scenario**
   - This have been tested and works on the device.
   - User can use the admin uA changer to exceed the max onscreen 500 uA level.
   - If level goes above 700, the device turns off and is disabled.
   - If therapy was ongoing, it is ended and recorded if selected.
   - The device enabled dropdown will change to false.
   - Won't be able to turn on the device in this state
   - Can use the dropdown to re-enable the device for simulation purposes by changing it to true.
   - Device uA will default back to 100 uA, and you can turn on the device again.
 
 ### Basic Use Case Steps for the Device
 
 1. Run the program
 2. Program will start with device in power on mode.
 3. Large timer will display 00:00.
 4. Not Treating, Not Recording and Contact Off labels will appear at top of device screen.
 5. Battery level will be displayed in upper left corner of the screen.
 6. Press select button to access menu for choosing therapy time duration.
 7. Use up/down buttons to navigate the menu.
 8. Press select again to lock in therapy time selection.
 9. Large time display will update to show the selected time.
 10. Selected time will be bolded above the large timer display. Other options are faded.
 11. Will now be on waveform selection menu.
 12. Repeat step 5.
 13. Press select again to lock in waveform selection.
 14. Selected waveform will be bolded. Other options are faded. 
 15. Waveform selection will update in admin area.
 16. Will now be on frequency selection menu.
 17. Repeat step 5.
 18. Press select again to lock in frequency selection.
 19. Selected frequency will be bolded. Other options are faded.
 20. Change skin contact dropdown in admin area from false to true.
 21. Therapy will begin.
 22. Large timer will begin counting down to zero.
 23. Treating, Contact On, and Timer On labels will appear on the screen.
 24. Press record button to record the therapy.
 25. Use the up/down buttons to change the power level during the therapy session.
 26. Battery level will deplete at a rate of 1% every 10-20 seconds depending on power level of therapy. (Higher power level drains Battery faster)
 27. Timer reaches zero, therapy ends.
 28. Not Treating, Contact Off labels will appear on screen.
 29. Timer On label will be faded.
 30. Skin contact in admin area dropdown will be changed to false.
 31. Power level will be reset to default level 2 (100 uA)
 32. Start time of the therapy, waveform, frequency, duration and last power level are stored for the therapy.
 33. Press Recorded Therapies Screen tab to view the recorded therapy.     

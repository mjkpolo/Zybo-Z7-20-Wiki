### Package design
---
The test design module needs to be placed in a directory alone, so it can be packaged:
![Screen_Shot_2022-03-16_at_3.39.10_PM](uploads/e5017c61bbdafa7979fd37729cd016c5/Screen_Shot_2022-03-16_at_3.39.10_PM.png)

Fill in the module with code:
![Screen_Shot_2022-03-16_at_3.43.04_PM](uploads/cdb885e46cb185dc43b067902d399137/Screen_Shot_2022-03-16_at_3.43.04_PM.png)

search package ip:
![Screen_Shot_2022-03-16_at_3.43.14_PM](uploads/2bc3979441c8edb8facfc293ffdaf0cc/Screen_Shot_2022-03-16_at_3.43.14_PM.png)

Click package:
![Screen_Shot_2022-03-16_at_3.45.10_PM](uploads/212517775ef54d93445140f8d4ca3c6f/Screen_Shot_2022-03-16_at_3.45.10_PM.png)

### Board design and constraints
---
create board design:
![Screen_Shot_2022-03-16_at_3.45.27_PM](uploads/b671a6eec058d2c1e7cb39afef0b7b3e/Screen_Shot_2022-03-16_at_3.45.27_PM.png)

add zynq and test module:
![Screen_Shot_2022-03-16_at_3.46.24_PM](uploads/d60281de0099a78eb6e67887300e8060/Screen_Shot_2022-03-16_at_3.46.24_PM.png)

Add constraints file from Xilinx:
![Screen_Shot_2022-03-16_at_3.50.35_PM](uploads/41cdfbc69571dd768ee6f89d5db1310f/Screen_Shot_2022-03-16_at_3.50.35_PM.png)

Find desired pins:
![Screen_Shot_2022-03-16_at_3.54.23_PM](uploads/0b263340b3ce24a9a51145905639852f/Screen_Shot_2022-03-16_at_3.54.23_PM.png)

Replace with test connections:
![Screen_Shot_2022-03-16_at_4.02.23_PM](uploads/572502bfb87ddd5049749e2681021125/Screen_Shot_2022-03-16_at_4.02.23_PM.png)

After running block and connection automation, right click the board design and create VHDL wrapper (Warning about pins will show):
![Screen_Shot_2022-03-16_at_4.01.35_PM](uploads/56e73754b320f625401e214415b6b41d/Screen_Shot_2022-03-16_at_4.01.35_PM.png)

To fix the warnings make the pins external and change them to the correct name:
![Screen_Shot_2022-03-16_at_4.09.57_PM](uploads/f6b6623f43950477cf420c92a3c5f386/Screen_Shot_2022-03-16_at_4.09.57_PM.png)

Run synthesis and implementation:
![Screen_Shot_2022-03-16_at_4.15.14_PM](uploads/affa5e9410a7fb88915eb39f88b470b5/Screen_Shot_2022-03-16_at_4.15.14_PM.png)


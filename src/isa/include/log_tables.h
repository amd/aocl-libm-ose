/*
 * Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __LOG_TABLES_H__
#define __LOG_TABLES_H__

.data


.align 16
.comm   L__ln_tail_64_table, 520
.type	L__ln_tail_64_table, @object
.size	L__ln_tail_64_table, 520
.L__ln_tail_64_table:
             .quad 0x0000000000000000   # 0.00000000000000000000e+00,   
             .quad 0x3e361f807c79f3db   # 5.15092497094772879206e-09,   
             .quad 0x3e6873c1980267c8   # 4.55457209735272790188e-08,   
             .quad 0x3e5ec65b9f88c69e   # 2.86612990859791781788e-08,   
             .quad 0x3e58022c54cc2f99   # 2.23596477332056055352e-08,   
             .quad 0x3e62c37a3a125330   # 3.49498983167142274770e-08,   
             .quad 0x3e615cad69737c93   # 3.23392843005887000414e-08,   
             .quad 0x3e4d256ab1b285e9   # 1.35722380472479366661e-08,   
             .quad 0x3e5b8abcb97a7aa2   # 2.56504325268044191098e-08,   
             .quad 0x3e6f34239659a5dc   # 5.81213608741512136843e-08,   
             .quad 0x3e6e07fd48d30177   # 5.59374849578288093334e-08,   
             .quad 0x3e6b32df4799f4f6   # 5.06615629004996189970e-08,   
             .quad 0x3e6c29e4f4f21cf8   # 5.24588857848400955725e-08,   
             .quad 0x3e1086c848df1b59   # 9.61968535632653505972e-10,   
             .quad 0x3e4cf456b4764130   # 1.34829655346594463137e-08,   
             .quad 0x3e63a02ffcb63398   # 3.65557749306383026498e-08,   
             .quad 0x3e61e6a6886b0976   # 3.33431709374069198903e-08,   
             .quad 0x3e6b8abcb97a7aa2   # 5.13008650536088382197e-08,   
             .quad 0x3e6b578f8aa35552   # 5.09285070380306053751e-08,   
             .quad 0x3e6139c871afb9fc   # 3.20853940845502057341e-08,   
             .quad 0x3e65d5d30701ce64   # 4.06713248643004200446e-08,   
             .quad 0x3e6de7bcb2d12142   # 5.57028186706125221168e-08,   
             .quad 0x3e6d708e984e1664   # 5.48356693724804282546e-08,   
             .quad 0x3e556945e9c72f36   # 1.99407553679345001938e-08,   
             .quad 0x3e20e2f613e85bda   # 1.96585517245087232086e-09,   
             .quad 0x3e3cb7e0b42724f6   # 6.68649386072067321503e-09,   
             .quad 0x3e6fac04e52846c7   # 5.89936034642113390002e-08,   
             .quad 0x3e5e9b14aec442be   # 2.85038578721554472484e-08,   
             .quad 0x3e6b5de8034e7126   # 5.09746772910284482606e-08,   
             .quad 0x3e6dc157e1b259d3   # 5.54234668933210171467e-08,   
             .quad 0x3e3b05096ad69c62   # 6.29100830926604004874e-09,   
             .quad 0x3e5c2116faba4cdd   # 2.61974119468563937716e-08,   
             .quad 0x3e665fcc25f95b47   # 4.16752115011186398935e-08,   
             .quad 0x3e5a9a08498d4850   # 2.47747534460820790327e-08,   
             .quad 0x3e6de647b1465f77   # 5.56922172017964209793e-08,   
             .quad 0x3e5da71b7bf7861d   # 2.76162876992552906035e-08,   
             .quad 0x3e3e6a6886b09760   # 7.08169709942321478061e-09,   
             .quad 0x3e6f0075eab0ef64   # 5.77453510221151779025e-08,   
             .quad 0x3e33071282fb989b   # 4.43021445893361960146e-09,   
             .quad 0x3e60eb43c3f1bed2   # 3.15140984357495864573e-08,   
             .quad 0x3e5faf06ecb35c84   # 2.95077445089736670973e-08,   
             .quad 0x3e4ef1e63db35f68   # 1.44098510263167149349e-08,   
             .quad 0x3e469743fb1a71a5   # 1.05196987538551827693e-08,   
             .quad 0x3e6c1cdf404e5796   # 5.23641361722697546261e-08,   
             .quad 0x3e4094aa0ada625e   # 7.72099925253243069458e-09,   
             .quad 0x3e6e2d4c96fde3ec   # 5.62089493829364197156e-08,   
             .quad 0x3e62f4d5e9a98f34   # 3.53090261098577946927e-08,   
             .quad 0x3e6467c96ecc5cbe   # 3.80080516835568242269e-08,   
             .quad 0x3e6e7040d03dec5a   # 5.66961038386146408282e-08,   
             .quad 0x3e67bebf4282de36   # 4.42287063097349852717e-08,   
             .quad 0x3e6289b11aeb783f   # 3.45294525105681104660e-08,   
             .quad 0x3e5a891d1772f538   # 2.47132034530447431509e-08,   
             .quad 0x3e634f10be1fb591   # 3.59655343422487209774e-08,   
             .quad 0x3e6d9ce1d316eb93   # 5.51581770357780862071e-08,   
             .quad 0x3e63562a19a9c442   # 3.60171867511861372793e-08,   
             .quad 0x3e54e2adf548084c   # 1.94511067964296180547e-08,   
             .quad 0x3e508ce55cc8c97a   # 1.54137376631349347838e-08,   
             .quad 0x3e30e2f613e85bda   # 3.93171034490174464173e-09,   
             .quad 0x3e6db03ebb0227bf   # 5.52990607758839766440e-08,   
             .quad 0x3e61b75bb09cb098   # 3.29990737637586136511e-08,   
             .quad 0x3e496f16abb9df22   # 1.18436010922446096216e-08,   
             .quad 0x3e65b3f399411c62   # 4.04248680368301346709e-08,   
             .quad 0x3e586b3e59f65355   # 2.27418915900284316293e-08,   
             .quad 0x3e52482ceae1ac12   # 1.70263791333409206020e-08,   
             .quad 0x3e6efa39ef35793c   # 5.76999904754328540596e-08;  



.align 16
.comm   L__ln_lead_64_table, 520
.type	L__ln_lead_64_table, @object
.size	L__ln_lead_64_table, 520
.L__ln_lead_64_table:
       .quad 0x0000000000000000   # 0.00000000000000000000e+00,
       .quad 0x3f8fc0a800000000   # 1.55041813850402832031e-02,
       .quad 0x3f9f829800000000   # 3.07716131210327148438e-02
       .quad 0x3fa7745800000000   # 4.58095073699951171875e-02
       .quad 0x3faf0a3000000000   # 6.06245994567871093750e-02
       .quad 0x3fb341d700000000   # 7.52233862876892089844e-02
       .quad 0x3fb6f0d200000000   # 8.96121263504028320312e-02
       .quad 0x3fba926d00000000   # 1.03796780109405517578e-01
       .quad 0x3fbe270700000000   # 1.17783010005950927734e-01
       .quad 0x3fc0d77e00000000   # 1.31576299667358398438e-01
       .quad 0x3fc2955280000000   # 1.45181953907012939453e-01
       .quad 0x3fc44d2b00000000   # 1.58604979515075683594e-01
       .quad 0x3fc5ff3000000000   # 1.71850204467773437500e-01
       .quad 0x3fc7ab8900000000   # 1.84922337532043457031e-01
       .quad 0x3fc9525a80000000   # 1.97825729846954345703e-01
       .quad 0x3fcaf3c900000000   # 2.10564732551574707031e-01
       .quad 0x3fcc8ff780000000   # 2.23143517971038818359e-01
       .quad 0x3fce270700000000   # 2.35566020011901855469e-01
       .quad 0x3fcfb91800000000   # 2.47836112976074218750e-01
       .quad 0x3fd0a324c0000000   # 2.59957492351531982422e-01
       .quad 0x3fd1675c80000000   # 2.71933674812316894531e-01
       .quad 0x3fd22941c0000000   # 2.83768117427825927734e-01
       .quad 0x3fd2e8e280000000   # 2.95464158058166503906e-01
       .quad 0x3fd3a64c40000000   # 3.07025015354156494141e-01
       .quad 0x3fd4618bc0000000   # 3.18453729152679443359e-01
       .quad 0x3fd51aad80000000   # 3.29753279685974121094e-01
       .quad 0x3fd5d1bd80000000   # 3.40926527976989746094e-01
       .quad 0x3fd686c800000000   # 3.51976394653320312500e-01
       .quad 0x3fd739d7c0000000   # 3.62905442714691162109e-01
       .quad 0x3fd7eaf800000000   # 3.73716354370117187500e-01
       .quad 0x3fd89a3380000000   # 3.84411692619323730469e-01
       .quad 0x3fd9479400000000   # 3.94993782043457031250e-01
       .quad 0x3fd9f323c0000000   # 4.05465066432952880859e-01
       .quad 0x3fda9cec80000000   # 4.15827870368957519531e-01
       .quad 0x3fdb44f740000000   # 4.26084339618682861328e-01
       .quad 0x3fdbeb4d80000000   # 4.36236739158630371094e-01
       .quad 0x3fdc8ff7c0000000   # 4.46287095546722412109e-01
       .quad 0x3fdd32fe40000000   # 4.56237375736236572266e-01
       .quad 0x3fddd46a00000000   # 4.66089725494384765625e-01
       .quad 0x3fde744240000000   # 4.75845873355865478516e-01
       .quad 0x3fdf128f40000000   # 4.85507786273956298828e-01
       .quad 0x3fdfaf5880000000   # 4.95077252388000488281e-01
       .quad 0x3fe02552a0000000   # 5.04556000232696533203e-01
       .quad 0x3fe0723e40000000   # 5.13945698738098144531e-01
       .quad 0x3fe0be72e0000000   # 5.23248136043548583984e-01
       .quad 0x3fe109f380000000   # 5.32464742660522460938e-01
       .quad 0x3fe154c3c0000000   # 5.41597247123718261719e-01,
       .quad 0x3fe19ee6a0000000   # 5.50647079944610595703e-01,
       .quad 0x3fe1e85f40000000   # 5.59615731239318847656e-01
       .quad 0x3fe23130c0000000   # 5.68504691123962402344e-01
       .quad 0x3fe2795e00000000   # 5.77315330505371093750e-01
       .quad 0x3fe2c0e9e0000000   # 5.86049020290374755859e-01
       .quad 0x3fe307d720000000   # 5.94707071781158447266e-01
       .quad 0x3fe34e2880000000   # 6.03290796279907226562e-01
       .quad 0x3fe393e0c0000000   # 6.11801505088806152344e-01
       .quad 0x3fe3d90260000000   # 6.20240390300750732422e-01
       .quad 0x3fe41d8fe0000000   # 6.28608644008636474609e-01
       .quad 0x3fe4618bc0000000   # 6.36907458305358886719e-01
       .quad 0x3fe4a4f840000000   # 6.45137906074523925781e-01
       .quad 0x3fe4e7d800000000   # 6.53301239013671875000e-01
       .quad 0x3fe52a2d20000000   # 6.61398470401763916016e-01
       .quad 0x3fe56bf9c0000000   # 6.69430613517761230469e-01
       .quad 0x3fe5ad4040000000   # 6.77398800849914550781e-01
       .quad 0x3fe5ee02a0000000   # 6.85303986072540283203e-01
       .quad 0x3fe62e42e0000000   # 6.93147122859954833984e-01

#endif


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

double sinh_lead[37] = {
            0.00000000000000000000e+00,  /* 0x0000000000000000 */
            1.17520119364380137839e+00,  /* 0x3ff2cd9fc44eb982 */
            3.62686040784701857476e+00,  /* 0x400d03cf63b6e19f */
            1.00178749274099008204e+01,  /* 0x40240926e70949ad */
            2.72899171971277496596e+01,  /* 0x403b4a3803703630 */
            7.42032105777887522891e+01,  /* 0x40528d0166f07374 */
            2.01713157370279219549e+02,  /* 0x406936d22f67c805 */
            5.48316123273246489589e+02,  /* 0x408122876ba380c9 */
            1.49047882578955000099e+03,  /* 0x409749ea514eca65 */
            4.05154190208278987484e+03,  /* 0x40afa7157430966f */
            1.10132328747033916443e+04,  /* 0x40c5829dced69991 */
            2.99370708492480553105e+04,  /* 0x40dd3c4488cb48d6 */
            8.13773957064298447222e+04,  /* 0x40f3de1654d043f0 */
            2.21206696003330085659e+05,  /* 0x410b00b5916a31a5 */
            6.01302142081972560845e+05,  /* 0x412259ac48bef7e3 */
            1.63450868623590236530e+06,  /* 0x4138f0ccafad27f6 */
            4.44305526025387924165e+06,  /* 0x4150f2ebd0a7ffe3 */
            1.20774763767876271158e+07,  /* 0x416709348c0ea4ed */
            3.28299845686652474105e+07,  /* 0x417f4f22091940bb */
            8.92411504815936237574e+07,  /* 0x419546d8f9ed26e1 */
            2.42582597704895108938e+08,  /* 0x41aceb088b68e803 */
            6.59407867241607308388e+08,  /* 0x41c3a6e1fd9eecfd */
            1.79245642306579566002e+09,  /* 0x41dab5adb9c435ff */
            4.87240172312445068359e+09,  /* 0x41f226af33b1fdc0 */
            1.32445610649217357635e+10,  /* 0x4208ab7fb5475fb7 */
            3.60024496686929321289e+10,  /* 0x4220c3d3920962c8 */
            9.78648047144193725586e+10,  /* 0x4236c932696a6b5c */
            2.66024120300899291992e+11,  /* 0x424ef822f7f6731c */
            7.23128532145737548828e+11,  /* 0x42650bba3796379a */
            1.96566714857202099609e+12,  /* 0x427c9aae4631c056 */
            5.34323729076223046875e+12,  /* 0x429370470aec28ec */
            1.45244248326237109375e+13,  /* 0x42aa6b765d8cdf6c */
            3.94814800913403437500e+13,  /* 0x42c1f43fcc4b662c */
            1.07321789892958031250e+14,  /* 0x42d866f34a725782 */
            2.91730871263727437500e+14,  /* 0x42f0953e2f3a1ef7 */
            7.93006726156715250000e+14,  /* 0x430689e221bc8d5a */
            2.15561577355759750000e+15   /* 0x431ea215a1d20d76 */
            };

double cosh_lead[37] = {
            1.00000000000000000000e+00,  /* 0x3ff0000000000000 */
            1.54308063481524371241e+00,  /* 0x3ff8b07551d9f550 */
            3.76219569108363138810e+00,  /* 0x400e18fa0df2d9bc */
            1.00676619957777653269e+01,  /* 0x402422a497d6185e */
            2.73082328360164865444e+01,  /* 0x403b4ee858de3e80 */
            7.42099485247878334349e+01,  /* 0x40528d6fcbeff3a9 */
            2.01715636122455890700e+02,  /* 0x406936e67db9b919 */
            5.48317035155212010977e+02,  /* 0x4081228949ba3a8b */
            1.49047916125217807348e+03,  /* 0x409749eaa93f4e76 */
            4.05154202549259389343e+03,  /* 0x40afa715845d8894 */
            1.10132329201033226127e+04,  /* 0x40c5829dd053712d */
            2.99370708659497577173e+04,  /* 0x40dd3c4489115627 */
            8.13773957125740562333e+04,  /* 0x40f3de1654d6b543 */
            2.21206696005590405548e+05,  /* 0x410b00b5916b6105 */
            6.01302142082804115489e+05,  /* 0x412259ac48bf13ca */
            1.63450868623620807193e+06,  /* 0x4138f0ccafad2d17 */
            4.44305526025399193168e+06,  /* 0x4150f2ebd0a8005c */
            1.20774763767876680940e+07,  /* 0x416709348c0ea503 */
            3.28299845686652623117e+07,  /* 0x417f4f22091940bf */
            8.92411504815936237574e+07,  /* 0x419546d8f9ed26e1 */
            2.42582597704895138741e+08,  /* 0x41aceb088b68e804 */
            6.59407867241607308388e+08,  /* 0x41c3a6e1fd9eecfd */
            1.79245642306579566002e+09,  /* 0x41dab5adb9c435ff */
            4.87240172312445068359e+09,  /* 0x41f226af33b1fdc0 */
            1.32445610649217357635e+10,  /* 0x4208ab7fb5475fb7 */
            3.60024496686929321289e+10,  /* 0x4220c3d3920962c8 */
            9.78648047144193725586e+10,  /* 0x4236c932696a6b5c */
            2.66024120300899291992e+11,  /* 0x424ef822f7f6731c */
            7.23128532145737548828e+11,  /* 0x42650bba3796379a */
            1.96566714857202099609e+12,  /* 0x427c9aae4631c056 */
            5.34323729076223046875e+12,  /* 0x429370470aec28ec */
            1.45244248326237109375e+13,  /* 0x42aa6b765d8cdf6c */
            3.94814800913403437500e+13,  /* 0x42c1f43fcc4b662c */
            1.07321789892958031250e+14,  /* 0x42d866f34a725782 */
            2.91730871263727437500e+14,  /* 0x42f0953e2f3a1ef7 */
            7.93006726156715250000e+14,  /* 0x430689e221bc8d5a */
            2.15561577355759750000e+15,  /* 0x431ea215a1d20d76 */
            };
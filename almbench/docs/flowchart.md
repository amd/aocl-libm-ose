# AMD LibM Test Suite - Flowcharts

**Generated:** October 10, 2025

---

## 1. Main Execution Flow

```mermaid
flowchart TD
    Start([Start: main.cc]) --> ParseArgs[Parse Command Line Arguments<br/>- shim_library<br/>- yaml_file<br/>- test_mode]
    ParseArgs --> LoadShim[Load Shim Library<br/>DL_LOAD shim_library]
    LoadShim --> ShimCheck{Library<br/>Loaded?}
    ShimCheck -->|No| Error1[Print Error & Exit]
    ShimCheck -->|Yes| LoadRef[Load Reference Library<br/>REF_MPARITH]
    LoadRef --> RefCheck{Library<br/>Loaded?}
    RefCheck -->|No| Error2[Print Error & Exit]
    RefCheck -->|Yes| ReadYAML[Read YAML Configuration<br/>read_yaml_file]
    ReadYAML --> ProcessLibM[Process LibM Tests<br/>process_libm]
    ProcessLibM --> Cleanup[Cleanup: DL_CLOSE libraries]
    Cleanup --> End([Exit])

    style Start fill:#e1f5e1
    style End fill:#ffe1e1
    style Error1 fill:#ffcccc
    style Error2 fill:#ffcccc
```

---

## 2. YAML Processing Flow

```mermaid
flowchart TD
    Start([read_yaml_file]) --> LoadYAML[Load YAML::LoadFile]
    LoadYAML --> IterSeq{For Each<br/>test_sequence}
    IterSeq -->|Next| ExtractFunc[Extract function name]
    ExtractFunc --> IterSet{For Each<br/>test_set}
    IterSet -->|Next| IterTest{For Each<br/>test}
    IterTest -->|Next| ReadTest[read_test<br/>Parse test configuration]
    ReadTest --> ParseInput[Parse input field]
    ParseInput --> InputType{Input<br/>Type?}
    InputType -->|Sequence| ParseRange[Parse as Range<br/>start, stop, type, count]
    InputType -->|Scalar| ParseSingle[Parse as Single Value]
    ParseRange --> ParseVariants
    ParseSingle --> ParseVariants[Parse variants<br/>ss;vrs4;sd;vrd2]
    ParseVariants --> ParseULP[Parse ULP Thresholds<br/>Default: 0.5 scalar, 4.0 vector]
    ParseULP --> ParseException[Parse Expected Exception]
    ParseException --> AddToVector[Add YamlInputs to vector]
    AddToVector --> IterTest
    IterTest -->|Done| IterSet
    IterSet -->|Done| IterSeq
    IterSeq -->|Done| Return([Return vector of YamlInputs])

    style Start fill:#e1f5e1
    style Return fill:#e1e1ff
```

---

## 3. Test Processing and Dispatch Flow

```mermaid
flowchart TD
    Start([process_libm]) --> GetVendor[Get Vendor Name<br/>from shim library]
    GetVendor --> IterParams{For Each<br/>YamlInputs}
    IterParams -->|Next| SplitVariants[Split variants by ';'<br/>e.g., ss;vrs4;sd]
    SplitVariants --> SplitULP[Split ULP thresholds by ';']
    SplitULP --> IterVariant{For Each<br/>Variant}

    IterVariant -->|ss| CallSS[libm_api_variant&lt;float&gt;]
    IterVariant -->|sd| CallSD[libm_api_variant&lt;double&gt;]
    IterVariant -->|vrs4| CallVRS4[libm_api_variant&lt;AlignedM128&gt;]
    IterVariant -->|vrs8| CallVRS8[libm_api_variant&lt;AlignedM256&gt;]
    IterVariant -->|vrs16| CheckAVX512{AVX-512<br/>Support?}
    CheckAVX512 -->|Yes| CallVRS16[libm_api_variant&lt;AlignedM512&gt;]
    CheckAVX512 -->|No| SkipVRS16[Skip: Not Supported]
    IterVariant -->|vrd2| CallVRD2[libm_api_variant&lt;AlignedM128d&gt;]
    IterVariant -->|vrd4| CallVRD4[libm_api_variant&lt;AlignedM256d&gt;]
    IterVariant -->|vrd8| CheckAVX512d{AVX-512<br/>Support?}
    CheckAVX512d -->|Yes| CallVRD8[libm_api_variant&lt;AlignedM512d&gt;]
    CheckAVX512d -->|No| SkipVRD8[Skip: Not Supported]
    IterVariant -->|vrsa/vrda| CallVRA[libm_api_variant&lt;T&gt;<br/>VRA Mode]

    CallSS --> IterVariant
    CallSD --> IterVariant
    CallVRS4 --> IterVariant
    CallVRS8 --> IterVariant
    CallVRS16 --> IterVariant
    SkipVRS16 --> IterVariant
    CallVRD2 --> IterVariant
    CallVRD4 --> IterVariant
    CallVRD8 --> IterVariant
    SkipVRD8 --> IterVariant
    CallVRA --> IterVariant

    IterVariant -->|Done| IterParams
    IterParams -->|Done| End([Return])

    style Start fill:#e1f5e1
    style End fill:#e1e1ff
```

---

## 4. API Variant Processing Flow

```mermaid
flowchart TD
    Start([libm_api_variant&lt;T,U&gt;]) --> CreateStructs[Create InParams&lt;T,U&gt;<br/>Create YamlOutputs&lt;U&gt;]
    CreateStructs --> SetupYOP[Setup YamlOutputs:<br/>- variant name<br/>- api_name<br/>- test_mode<br/>- ulp_threshold]
    SetupYOP --> CheckRange{Has Range<br/>Input?}

    CheckRange -->|Yes| IterRanges{For Each<br/>Range}
    IterRanges -->|Next| ConvertRange[Convert String to Float<br/>str2flt: handle hex, special values]
    ConvertRange --> CreateInpRng[Create InpRng&lt;U&gt;<br/>- start, stop<br/>- type, count]
    CreateInpRng --> AddRange[Add to ipp->range vector]
    AddRange --> IterRanges
    IterRanges -->|Done| CheckExpected

    CheckRange -->|No| ParseSingleInput[Parse Single Input Values<br/>str2flt for each input]
    ParseSingleInput --> StoreIP[Store in ipp->ip array]
    StoreIP --> CheckExpected

    CheckExpected{Has Expected<br/>Value?}
    CheckExpected -->|Yes| ParseExpected[Parse Expected Output<br/>ipp->xv = str2flt]
    CheckExpected -->|No| CheckException
    ParseExpected --> CheckException

    CheckException{Has Expected<br/>Exception?}
    CheckException -->|Yes| ParseExcept[Parse Exception Flags<br/>get_exception_flag]
    CheckException -->|No| CallValidate
    ParseExcept --> CallValidate[Call validate_api&lt;T,U&gt;]

    CallValidate --> Cleanup[Delete ipp and yop]
    Cleanup --> End([Return])

    style Start fill:#e1f5e1
    style End fill:#e1e1ff
```

---

## 5. API Validation and Routing Flow

```mermaid
flowchart TD
    Start([validate_api&lt;T,U&gt;]) --> SetVRA[Set is_vra flag<br/>Check if variant ends with 'a']
    SetVRA --> CheckOutDir[check_outfile_dir<br/>Create dumps directory]
    CheckOutDir --> LookupAPI[get_api_prototype<br/>Look up API type from name]

    LookupAPI --> APIType{API<br/>Prototype<br/>Type?}

    APIType -->|PROTOTYPE_01| BuildName01[Build Function Names<br/>libapi = api_ss<br/>refapi = mp_api]
    APIType -->|PROTOTYPE_02| BuildName02[Build Function Names<br/>Two inputs]
    APIType -->|PROTOTYPE_03| BuildName03[Build Function Names<br/>Scalar second input]
    APIType -->|PROTOTYPE_04| BuildName04[Build Function Names<br/>Dual outputs]
    APIType -->|PROTOTYPE_05| BuildName05[Build Function Names<br/>Six inputs]
    APIType -->|PROTOTYPE_06| BuildName06[Build Function Names<br/>Integer input]
    APIType -->|Unknown| ErrorAPI[Print Error:<br/>API not supported]

    BuildName01 --> Call01[api_prototype_01&lt;T,U&gt;]
    BuildName02 --> Call02[api_prototype_02&lt;T,U&gt;]
    BuildName03 --> Call03[api_prototype_03&lt;T,U&gt;]
    BuildName04 --> Call04[api_prototype_04&lt;T,U&gt;]
    BuildName05 --> Call05[api_prototype_05&lt;T,U&gt;]
    BuildName06 --> Call06[api_prototype_06&lt;T,U&gt;]

    Call01 --> Return
    Call02 --> Return
    Call03 --> Return
    Call04 --> Return
    Call05 --> Return
    Call06 --> Return
    ErrorAPI --> Return([Return])

    style Start fill:#e1f5e1
    style Return fill:#e1e1ff
    style ErrorAPI fill:#ffcccc
```

---

## 6. API Prototype Execution Flow (Example: Prototype 01)

```mermaid
flowchart TD
    Start([api_prototype_01&lt;T,U&gt;]) --> LoadSymbols[Load Function Symbols<br/>- shim_func from shim lib<br/>- ref_func from reference lib]
    LoadSymbols --> CheckMode{Range<br/>Empty?}

    CheckMode -->|Yes: Unit Test| UnitTest[unit_test]
    CheckMode -->|No: Range Test| CheckVRA{is_vra<br/>flag?}

    CheckVRA -->|No| RangeTest[range_test<br/>SIMD/Scalar mode]
    CheckVRA -->|Yes| RangeTestVRA[range_test_vra<br/>Array pointer mode]

    UnitTest --> UTEnd
    RangeTest --> UTEnd
    RangeTestVRA --> UTEnd([Return 0])

    style Start fill:#e1f5e1
    style UTEnd fill:#e1e1ff
```

---

## 7. Unit Test Execution Flow

```mermaid
flowchart TD
    Start([unit_test]) --> RunExcept[run_libm_api_with_exceptions<br/>Execute shim, capture FP exceptions]
    RunExcept --> ExtractIO[Extract Input/Output Pointers<br/>ip = &ipp->ip<br/>op = &ipp->op]
    ExtractIO --> CallRef[Call Reference Function<br/>mpfrop = ref_func ip]
    CallRef --> ComputeULP[Compute ULP Error<br/>update_ulp]
    ComputeULP --> PopulateYOP[Populate YamlOutputs<br/>- iptr, optr<br/>- ulp, status<br/>- exception_raised]
    PopulateYOP --> WriteYAML[write_yaml_output<br/>Serialize to YAML file]
    WriteYAML --> End([Return])

    style Start fill:#e1f5e1
    style End fill:#e1e1ff
```

---

## 8. Range Test Execution Flow (Non-VRA)

```mermaid
flowchart TD
    Start([range_test]) --> CalcElem[Calculate SIMD Element Count<br/>elem = sizeof T / sizeof U]
    CalcElem --> AlignCount[Align Input Count to SIMD<br/>count = align_to range.count, elem]
    AlignCount --> SetupYOP[Setup YamlOutputs<br/>- n = elem<br/>- ulp, status arrays]
    SetupYOP --> CreateRunner[Create Runner&lt;T,U&gt;<br/>Mode: Accuracy or Performance]
    CreateRunner --> CreateGen[Create MultiStepGenerator<br/>range.srt, range.stp<br/>range.type, range.count]
    CreateGen --> CreatePacker[Create FloatPacker&lt;T&gt;]
    CreatePacker --> LoopStart{For i = 0<br/>to N}

    LoopStart -->|Continue| GenNext[ip = val.wrap_next<br/>Get next SIMD-width inputs]
    GenNext --> Pack[ipp->ip = fp.pack ip<br/>Pack into SIMD register]
    Pack --> RunFunc[duration = runner.run ipp<br/>Execute shim function]
    RunFunc --> LoopElem{For j = 0<br/>to elem}

    LoopElem -->|Continue| CallRefElem[mpfrop = ref_func ip j]
    CallRefElem --> UpdateULPElem[status j = update_ulp<br/>op j, mpfrop, ulp j]
    UpdateULPElem --> LoopElem

    LoopElem -->|Done| SetPointers[Set yop pointers<br/>iptr, optr]
    SetPointers --> WriteOutput[write_yaml_output yop]
    WriteOutput --> LoopStart

    LoopStart -->|Done| End([Return])

    style Start fill:#e1f5e1
    style End fill:#e1e1ff
```

---

## 9. Range Test VRA Execution Flow

```mermaid
flowchart TD
    Start([range_test_vra]) --> CalcElem[Calculate Element Count<br/>elem = sizeof T / sizeof U]
    CalcElem --> SetCount[Set count = min range.count, 100<br/>Limit batch size]
    SetCount --> AlignN[Align N = align_to count, elem]
    AlignN --> AllocVectors[Allocate Output Vectors<br/>std::vector&lt;U&gt; op count<br/>std::vector&lt;double&gt; ulp count<br/>std::vector&lt;int&gt; status count]
    AllocVectors --> SetupIPP[Setup InParams<br/>ipp->count = count<br/>ipp->optr = op.data]
    SetupIPP --> SetupYOP[Setup YamlOutputs<br/>yop->n = count<br/>yop->optr, ulp, status]
    SetupYOP --> CreateRunner[Create Runner&lt;T,U&gt;]
    CreateRunner --> CreateGen[Create MultiStepGenerator<br/>for count elements]
    CreateGen --> LoopStart{For i = 0<br/>to N}

    LoopStart -->|Continue| GenNext[ip = val.wrap_next<br/>Get next batch of inputs]
    GenNext --> SetIPPtr[ipp->iptr = ip<br/>Set pointer to input array]
    SetIPPtr --> RunFunc[duration = runner.run ipp<br/>Execute shim function]
    RunFunc --> LoopElem{For j = 0<br/>to count}

    LoopElem -->|Continue| CallRefElem[mpfrop = ref_func ip j]
    CallRefElem --> UpdateULPElem[status j = update_ulp<br/>op j, mpfrop, ulp j]
    UpdateULPElem --> LoopElem

    LoopElem -->|Done| SetPtrs[Set yop->iptr = ip]
    SetPtrs --> WriteOutput[write_yaml_output yop]
    WriteOutput --> LoopStart

    LoopStart -->|Done| End([Return])

    style Start fill:#e1f5e1
    style End fill:#e1e1ff
```

---

## 10. Runner Execution Flow

```mermaid
flowchart TD
    Start([Runner::run]) --> CheckMode{Test<br/>Mode?}

    CheckMode -->|E_PERFORMANCE| RunPerf[run_perf]
    CheckMode -->|E_ACCURACY| RunAccu[run_accu]

    RunPerf --> InitDurations[Initialize durations vector<br/>reserve iterations]
    InitDurations --> PerfLoop{For t = 0<br/>to iterations}
    PerfLoop -->|Continue| StartTimer[timing_wrapper.start]
    StartTimer --> ExecShim1[shim_func ipp]
    ExecShim1 --> StopTimer[duration = timing_wrapper.stop]
    StopTimer --> StoreDuration[durations.push_back duration]
    StoreDuration --> PerfLoop
    PerfLoop -->|Done| FindMin[Return min_element durations]
    FindMin --> ReturnPerf

    RunAccu --> ExecShim2[shim_func ipp]
    ExecShim2 --> Return0[Return 0.0]
    Return0 --> ReturnPerf([Return duration])
    FindMin --> ReturnPerf

    style Start fill:#e1f5e1
    style ReturnPerf fill:#e1e1ff
```

---

## 11. ULP Calculation Flow

```mermaid
flowchart TD
    Start([update_ulp]) --> CallCompute[ulp = compute_ulp<br/>actual, expected]
    CallCompute --> CheckInfNaN{ulp is<br/>Inf or NaN?}

    CheckInfNaN -->|Yes| SetFail1[res = TESTCASE_FAIL]
    CheckInfNaN -->|No| UpdateMax[udata.max_ulp_err =<br/>max max_ulp_err, ulp]
    UpdateMax --> CheckThreshold{ulp ><br/>threshold?}

    CheckThreshold -->|Yes| SetFail2[res = TESTCASE_FAIL]
    CheckThreshold -->|No| SetPass[res = TESTCASE_PASS]

    SetFail1 --> Return
    SetFail2 --> Return
    SetPass --> Return([Return res])

    style Start fill:#e1f5e1
    style Return fill:#e1e1ff
    style SetFail1 fill:#ffcccc
    style SetFail2 fill:#ffcccc
    style SetPass fill:#ccffcc
```

---

## 12. Compute ULP Flow

```mermaid
flowchart TD
    Start([compute_ulp]) --> CheckBothNaN{Both<br/>NaN?}
    CheckBothNaN -->|Yes| Return0A[Return 0.0]
    CheckBothNaN -->|No| CheckEitherNaN{Either<br/>NaN?}
    CheckEitherNaN -->|Yes| ReturnInf1[Return INFINITY]
    CheckEitherNaN -->|No| CheckBothInf{Both<br/>+Inf or<br/>Both -Inf?}
    CheckBothInf -->|Yes| Return0B[Return 0.0]
    CheckBothInf -->|No| CheckActualInf{Actual<br/>Infinite?}

    CheckActualInf -->|Yes| CheckExpInf{Expected<br/>Infinite or<br/>> max?}
    CheckExpInf -->|Yes| Return0C[Return 0.0]
    CheckExpInf -->|No| CalcInfULP[Calculate overflow ULP<br/>abs max - exp / ulprep]
    CalcInfULP --> ReturnInfULP[Return ulp + 1]

    CheckActualInf -->|No| CheckActualFinite{Actual<br/>Finite?}
    CheckActualFinite -->|Yes| CheckExpFinite{Expected<br/>Finite?}
    CheckExpFinite -->|Yes| CalcNormal[Calculate normal ULP<br/>abs actual - exp / ulprep exp]
    CalcNormal --> ReturnNormal[Return ulp]

    CheckExpFinite -->|No: Exp > max| CalcOverflow[Calculate overflow<br/>abs actual - max / ulprep + 1]
    CalcOverflow --> ReturnOverflow[Return ulp]

    CheckActualFinite -->|No| Return0D[Return 0.0]

    Return0A --> End
    ReturnInf1 --> End
    Return0B --> End
    Return0C --> End
    ReturnInfULP --> End
    ReturnNormal --> End
    ReturnOverflow --> End
    Return0D --> End([Return ulp])

    style Start fill:#e1f5e1
    style End fill:#e1e1ff
```

---

## 13. YAML Output Serialization Flow

```mermaid
flowchart TD
    Start([write_yaml_output]) --> Serialize[serialize_yaml_outputs<br/>Create YAML::Node]
    Serialize --> SetBasic[Set Basic Fields<br/>- api, n, test_id]
    SetBasic --> CheckUT{utflag<br/>mode?}

    CheckUT -->|Yes: Unit Test| SerializeSingleIP[Serialize Single Input<br/>ip: to_hex iptr 0]
    CheckUT -->|No: Batch| SerializeArrayIP[Serialize Array Inputs<br/>Loop over n elements]

    SerializeSingleIP --> SerializeSingleOP
    SerializeArrayIP --> SerializeArrayOP

    SerializeSingleOP[Serialize Single Output<br/>op: to_hex optr 0]
    SerializeArrayOP[Serialize Array Outputs<br/>Loop over n elements]

    SerializeSingleOP --> SerializeSingleULP[Serialize Single ULP<br/>ulp: to_hex ulp 0]
    SerializeArrayOP --> SerializeArrayULP[Serialize Array ULP<br/>Loop over n elements]

    SerializeSingleULP --> SerializeSingleStatus[Serialize Single Status<br/>status: PASS/FAIL]
    SerializeArrayULP --> SerializeArrayStatus[Serialize Array Status<br/>Loop over n elements]

    SerializeSingleStatus --> AddException[Add exception_raised<br/>exception_to_string]
    SerializeArrayStatus --> CheckPerfMode{Test Mode<br/>Performance?}

    CheckPerfMode -->|Yes| AddDuration[Add duration field]
    CheckPerfMode -->|No| SkipDuration
    AddDuration --> PrintStdout
    SkipDuration --> OpenFile

    AddException --> PrintStdout[Print to stdout<br/>YAML::Emitter]
    PrintStdout --> OpenFile

    OpenFile[Open Output File<br/>append mode]
    OpenFile --> WriteNode[Write node to file]
    WriteNode --> CloseFile[Close file]
    CloseFile --> End([Return])

    style Start fill:#e1f5e1
    style End fill:#e1e1ff
```

---

## 14. Input Generator Flow (MultiStepGenerator)

```mermaid
flowchart TD
    Start([MultiStepGenerator<br/>Constructor]) --> AlignCount[Align count to elem boundary<br/>aligned_count = align_to count, elem]
    AlignCount --> CheckType{Range<br/>Type?}

    CheckType -->|E_Simple/E_Linear| CreateLinear[Create LinearGenerator<br/>start, stop, aligned_count]
    CheckType -->|E_Expstep| CreateExp[Create ExponentialGenerator<br/>start, stop, aligned_count]
    CheckType -->|E_Bitstep| CreateBit[Create BitGenerator<br/>start, stop, bitstep=1]
    CheckType -->|E_Random| CreateRand[Create RandomGenerator<br/>start, stop, count, seed=42]
    CheckType -->|E_Integer| CreateInt[Create IntegerGenerator<br/>start, stop, aligned_count]
    CheckType -->|E_Fixedval| CreateFixed[Create FixedValueGenerator<br/>value, aligned_count]
    CheckType -->|Unknown| ThrowError[Throw exception:<br/>Unsupported RangeType]

    CreateLinear --> StoreGen[Store unique_ptr to generator]
    CreateExp --> StoreGen
    CreateBit --> StoreGen
    CreateRand --> StoreGen
    CreateInt --> StoreGen
    CreateFixed --> StoreGen

    StoreGen --> End([Constructor Complete])
    ThrowError --> End

    style Start fill:#e1f5e1
    style End fill:#e1e1ff
    style ThrowError fill:#ffcccc
```

---

## 15. Exception Handling Flow

```mermaid
flowchart TD
    Start([run_libm_api_with_exceptions]) --> ClearExcept[std::feclearexcept<br/>FE_ALL_EXCEPT]
    ClearExcept --> ExecShim[shim_func ipp<br/>Execute function under test]
    ExecShim --> TestExcept[raised = std::fetestexcept<br/>FE_ALL_EXCEPT]
    TestExcept --> ClearAgain[std::feclearexcept<br/>FE_ALL_EXCEPT]
    ClearAgain --> Return([Return raised flags])

    style Start fill:#e1f5e1
    style Return fill:#e1e1ff
```

---

## 16. String to Float Conversion Flow

```mermaid
flowchart TD
    Start([str2flt]) --> CheckSpecial{String<br/>Value?}

    CheckSpecial -->|"max"| SetMax[rhs = numeric_limits::max]
    CheckSpecial -->|"min"| SetMin[rhs = numeric_limits::min]
    CheckSpecial -->|"min_subnormal"| SetMinSub[rhs = denorm_min]
    CheckSpecial -->|"max_subnormal"| SetMaxSub[rhs = min - denorm_min]
    CheckSpecial -->|"qnan"| SetQNaN[rhs = quiet_NaN]
    CheckSpecial -->|"snan"/"nan"| SetSNaN[rhs = signaling_NaN]
    CheckSpecial -->|"inf"| SetInf[rhs = infinity]
    CheckSpecial -->|Other| CheckFormat{Format<br/>Type?}

    CheckFormat -->|Decimal/Scientific| ParseDecimal[std::stof / std::stod<br/>value]
    CheckFormat -->|Hexadecimal| ParseHex[std::stoul / std::stoull<br/>base 16]
    ParseHex --> MemCopy[std::memcpy to float/double]

    SetMax --> ReturnTrue
    SetMin --> ReturnTrue
    SetMinSub --> ReturnTrue
    SetMaxSub --> ReturnTrue
    SetQNaN --> ReturnTrue
    SetSNaN --> ReturnTrue
    SetInf --> ReturnTrue
    ParseDecimal --> ReturnTrue
    MemCopy --> ReturnTrue([Return true])

    ParseDecimal -.Invalid.-> CatchError[Catch exception<br/>Return false]
    ParseHex -.Invalid.-> CatchError
    CatchError --> ReturnFalse([Return false])

    style Start fill:#e1f5e1
    style ReturnTrue fill:#ccffcc
    style ReturnFalse fill:#ffcccc
```

---

## 17. Complete Test Lifecycle

```mermaid
flowchart TD
    Start([Test Suite Lifecycle]) --> Phase1[Phase 1: Initialization<br/>- Parse arguments<br/>- Load libraries<br/>- Read YAML config]
    Phase1 --> Phase2[Phase 2: Test Generation<br/>- Parse test specifications<br/>- Convert strings to values<br/>- Create input ranges]
    Phase2 --> Phase3[Phase 3: Variant Iteration<br/>- For each API variant<br/>- Create typed structures<br/>- Setup output files]
    Phase3 --> Phase4[Phase 4: API Dispatch<br/>- Lookup prototype type<br/>- Load function symbols<br/>- Route to handler]
    Phase4 --> Phase5{Phase 5: Test Execution}

    Phase5 -->|Unit Test| UT[Execute single input<br/>Capture exceptions]
    Phase5 -->|Range Test| RT[Generate input batch<br/>Execute multiple times]
    Phase5 -->|VRA Test| VRA[Execute array API<br/>Process batch]

    UT --> Phase6
    RT --> Phase6
    VRA --> Phase6

    Phase6[Phase 6: Verification<br/>- Call reference function<br/>- Compute ULP error<br/>- Check threshold]
    Phase6 --> Phase7[Phase 7: Output<br/>- Serialize to YAML<br/>- Write to file<br/>- Print diagnostics]
    Phase7 --> CheckMore{More<br/>Tests?}
    CheckMore -->|Yes| Phase3
    CheckMore -->|No| Phase8[Phase 8: Cleanup<br/>- Close libraries<br/>- Free memory<br/>- Exit]
    Phase8 --> End([End])

    style Start fill:#e1f5e1
    style End fill:#ffe1e1
```

---

## 18. Data Structure Relationships

```mermaid
graph TD
    AlmLibs[AlmLibs<br/>Library Handles] -->|Used by| ValidateAPI[validate_api]
    YamlInputs[YamlInputs<br/>Test Configuration] -->|Parsed into| InParams[InParams&lt;T,U&gt;<br/>Execution Parameters]
    InParams -->|Contains| IPArray[ip array<br/>Single values]
    InParams -->|Contains| IPPtr[iptr array<br/>Array pointers]
    InParams -->|Contains| OPPtr[optr array<br/>Output pointers]
    InParams -->|Contains| Range[range vector<br/>InpRng&lt;U&gt;]

    InParams -->|Passed to| Prototype[api_prototype_XX]

    YamlOutputs[YamlOutputs&lt;U&gt;<br/>Test Results] -->|Populated by| Prototype
    YamlOutputs -->|Contains| IPtrOut[iptr array<br/>Input snapshots]
    YamlOutputs -->|Contains| OPtrOut[optr array<br/>Output snapshots]
    YamlOutputs -->|Contains| ULPData[ulp array<br/>Error values]
    YamlOutputs -->|Contains| StatusData[status array<br/>PASS/FAIL]

    YamlOutputs -->|Serialized by| WriteYAML[write_yaml_output]

    Generator[IGenerator&lt;S&gt;] -->|Creates| InputValues[Input Value Sequence]
    InputValues -->|Stored in| IPPtr

    Runner[Runner&lt;T,U&gt;] -->|Executes| ShimFunc[Shim Function]
    Runner -->|Uses| TimingWrapper[timing_wrapper]

    style AlmLibs fill:#ffe1e1
    style YamlInputs fill:#e1ffe1
    style InParams fill:#e1e1ff
    style YamlOutputs fill:#ffe1ff
    style Generator fill:#ffffe1
```

---

## 19. Performance Test Optimization Flow

```mermaid
flowchart TD
    Start([Performance Test]) --> WarmUp[Warm-up Iteration<br/>Cache priming]
    WarmUp --> InitVector[Initialize durations vector<br/>Reserve iterations count]
    InitVector --> LoopStart{Iteration i<br/>< iterations?}

    LoopStart -->|Yes| ClearCache[Optional: Clear cache]
    ClearCache --> StartTimer[Start high-res timer]
    StartTimer --> ExecFunction[Execute shim function<br/>on input batch]
    ExecFunction --> StopTimer[Stop timer<br/>Record nanoseconds]
    StopTimer --> StoreDuration[Store duration i]
    StoreDuration --> Increment[i++]
    Increment --> LoopStart

    LoopStart -->|No| SortDurations[Sort durations vector]
    SortDurations --> SelectMetric{Metric<br/>Selection}

    SelectMetric -->|Minimum| GetMin[Return min_element<br/>Best case timing]
    SelectMetric -->|Median| GetMedian[Return median<br/>Typical case]
    SelectMetric -->|Mean| GetMean[Return average<br/>Expected case]

    GetMin --> ComputeThroughput[Compute Throughput<br/>ops/sec = n / duration]
    GetMedian --> ComputeThroughput
    GetMean --> ComputeThroughput

    ComputeThroughput --> Return([Return metrics])

    style Start fill:#e1f5e1
    style Return fill:#e1e1ff
```

---

## 20. Error Handling and Recovery Flow

```mermaid
flowchart TD
    Start([Error Detection Point]) --> ErrorType{Error<br/>Type?}

    ErrorType -->|Library Load Failure| LibError[Print library path<br/>Print DL_ERROR]
    LibError --> ExitProgram[Exit with code 1]

    ErrorType -->|Symbol Not Found| SymError[Print symbol name<br/>Print DL_ERROR]
    SymError --> CloseLib[DL_CLOSE library]
    CloseLib --> ExitProgram

    ErrorType -->|YAML Parse Error| YAMLError[Catch yaml-cpp exception<br/>Print error message]
    YAMLError --> SkipTest[Skip malformed test<br/>Continue with next]

    ErrorType -->|Invalid Range| RangeError[Throw std::invalid_argument<br/>Unsupported RangeType]
    RangeError --> CatchOuter[Catch in outer scope<br/>Print diagnostics]
    CatchOuter --> SkipTest

    ErrorType -->|File I/O Error| IOError[Print warning message<br/>Cannot create output]
    IOError --> ContinueTest[Continue test execution<br/>No output written]

    ErrorType -->|ULP Computation Error| ULPError[Check for Inf/NaN result<br/>Mark as TESTCASE_FAIL]
    ULPError --> LogFailure[Log failure details<br/>PrintUlpResults]
    LogFailure --> ContinueLoop[Continue with next input]

    ErrorType -->|Unsupported Variant| VariantError[Print skip message<br/>AVX-512 not available]
    VariantError --> SkipVariant[Skip to next variant]

    SkipTest --> RecoverPoint
    ContinueTest --> RecoverPoint
    ContinueLoop --> RecoverPoint
    SkipVariant --> RecoverPoint[Recovery Point<br/>Continue processing]

    RecoverPoint --> End([Return to main flow])
    ExitProgram --> TermEnd([Program Termination])

    style Start fill:#ffe1e1
    style End fill:#e1e1ff
    style TermEnd fill:#ffcccc
    style ExitProgram fill:#ffcccc
```

---

## Summary

These flowcharts illustrate the complete execution flow of the AMD LibM test suite, from command-line parsing through test execution and result serialization. Key aspects covered:

1. **Main execution paths** - Initialization, library loading, and cleanup
2. **YAML processing** - Configuration parsing and test generation
3. **Variant dispatch** - Routing to appropriate type-specific handlers
4. **Test execution modes** - Unit tests, range tests, and VRA tests
5. **ULP verification** - Accuracy computation and threshold checking
6. **Performance measurement** - Timing and throughput calculation
7. **Data flow** - Structure relationships and transformations
8. **Error handling** - Recovery strategies for various failure modes

The modular design allows for independent testing of each component while maintaining a clear overall execution sequence.

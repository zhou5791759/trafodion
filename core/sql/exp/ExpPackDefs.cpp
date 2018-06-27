/**********************************************************************
// @@@ START COPYRIGHT @@@
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
// @@@ END COPYRIGHT @@@
**********************************************************************/
/* -*-C++-*-
****************************************************************************
*
* File:         ExpPackDefs.cpp
* Description:  pack() and unpack() for exp_expr and exp_clause classes.
*
* Created:      5/6/98
* Language:     C++
*
*
*
****************************************************************************
*/

#include "Platform.h"


#include "exp_stdh.h"
#include "exp_clause_derived.h"
#include "exp_function.h"
#include "exp_math_func.h"
#include "exp_attrs.h"
#include "ExpBitMuxFunction.h"
#include "ExpSequenceFunction.h"
#include "ExpPCode.h"

// -----------------------------------------------------------------------
// P A C K
// -----------------------------------------------------------------------


/////////////////////////////////////////////////////
// pack procs for classes ex_expr, OutputExpr
/////////////////////////////////////////////////////
void ex_expr::packStuff(void * space_)
{
  //Following code packs ex_clause elements iteratively.
  ex_clause *keepClause = NULL;
  ex_clause *currClause = clauses_;
  ex_clause *nextClause;
  ExClausePtr currClausePtr;
  
  while( currClause != NULL ){
    currClausePtr = currClause;
    nextClause = currClause->getNextClause();
    currClausePtr.pack(space_);
    //Store the offset generated by currClausePtr.pack()  
    //back into the ExClausePtr element. During first pass,
    //keepClause is NULL for first element. Rest of the passes,
    //keepClause is the previous packed ex_clause which requires 
    //offset updated within its NextClause_ member.
    if(keepClause)
      keepClause->setNextPackedClause(currClausePtr.getOffset());
    else
      clauses_ = currClausePtr.getOffset();
    keepClause = currClause;
    currClause = nextClause;
  }
  //Continue packing rest of the members.
  constantsArea_.pack(space_);
  tempsArea_.pack(space_);
  persistentArea_.pack(space_);
  persistentInitializationArea_.pack(space_);
  pCode_.pack(space_);  
}

Long ex_expr::pack(void * space_)
{
  packStuff(space_);
  return NAVersionedObject::pack(space_);
}

void ex_expr_lean::convAddrToOffsetInPCode(void * space) {
  PCodeBinary *pcode = getPCodeBinary();
  if (!pcode)
    return;

  Int32 length = *(pcode++);
  pcode += (2 * length);

  while (pcode[0] != PCIT::END) {
    Int32 addrBuf[6];
    Int32 *addrs = PCode::getEmbeddedAddresses(pcode[0], addrBuf);
    for(Int32 i = 0; addrs[i] > 0; i++) {
      pcode[addrs[i]] = ((Space*)space)->convertToOffset((char*)*(Long*)&(pcode[addrs[i]]));
#ifndef NA_LITTLE_ENDIAN
      //swapInt64((char*)(&(pcode[addrs[i]])));
#endif
    }
    pcode += PCode::getInstructionLength(pcode);
  }

  pCodeLean_ = (PCodeBinary*)((Space *)space)->convertToOffset((char*)pCodeLean_);
}

Long ex_expr_lean::pack(void * space_)
{
  constantsArea_.pack(space_);
  tempsArea_.pack(space_);
  persistentArea_.pack(space_);

  convAddrToOffsetInPCode(space_);
  //  pCodeLean_.pack(space_);  

  return NAVersionedObject::pack(space_);
}

Long AggrExpr::pack(void * space_)
{
  initExpr_.pack(space_);
  perrecExpr_.pack(space_);
  finalNullExpr_.pack(space_);
  finalExpr_.pack(space_);
  groupingExpr_.pack(space_);

  return ex_expr::pack(space_);
}

Long InputOutputExpr::pack(void * space_)
{
  return ex_expr::pack(space_);
}

//////////////////////////////////////////////
// pack procs for clauses
//////////////////////////////////////////////

Long ex_aggregate_clause::pack(void * space_)
{
  return packClause(space_, sizeof(ex_aggregate_clause));
}

Long ex_arith_clause::pack(void * space_)
{
  return packClause(space_, sizeof(ex_arith_clause));
}

Long ex_unlogic_clause::pack(void * space_)
{
  return packClause(space_, sizeof(ex_unlogic_clause));
}

Long ExRegexpClauseChar::pack(void * space_)
{
  return packClause(space_, sizeof(ExRegexpClauseChar));
}

Long ex_like_clause_char::pack(void * space_)
{
  return packClause(space_, sizeof(ex_like_clause_char));
}

Long ex_like_clause_doublebyte::pack(void * space_)
{
  return packClause(space_, sizeof(ex_like_clause_doublebyte));
}

Long ex_bool_clause::pack(void * space_)
{
  return packClause(space_, sizeof(ex_bool_clause));
}

Long bool_result_clause::pack(void * space_)
{
  return packClause(space_, sizeof(bool_result_clause));
}

Long ex_branch_clause::pack(void * space_)
{
  saved_next_clause.pack(space_);
  branch_clause.pack(space_);
  return packClause(space_, sizeof(ex_branch_clause));
}

Long ex_comp_clause::pack(void * space_)
{
  return packClause(space_, sizeof(ex_comp_clause));
}

Long ex_conv_clause::pack(void * space_)
{
  return packClause(space_, sizeof(ex_conv_clause));
}

Long ex_function_clause::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_clause));
}

Long ex_function_concat::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_concat));
}  

Long ex_function_substring::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_substring));
}  

Long ex_function_substring_doublebyte::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_substring_doublebyte));
}  

Long ex_function_trim_char::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_trim_char));
}  

Long ex_function_trim_doublebyte::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_trim_doublebyte));
}  

Long ex_function_lower::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_lower));
}  

Long ex_function_upper::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_upper));
}  

Long ex_function_translate::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_translate));
}  

Long ExFunctionAscii::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionAscii));
}

Long ExFunctionChar::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionChar));
}

Long ExFunctionConvertHex::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionConvertHex));
}

Long ExFunctionRepeat::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionRepeat));
}

Long ExFunctionReplace::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionReplace));
}

// 12/17/97: added for unicode UPPER()
Long ex_function_upper_unicode::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_upper_unicode));
}  

Long ex_function_lower_unicode::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_lower_unicode));
}  

Long ex_function_char_length::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_char_length));
}  

Long ex_function_char_length_doublebyte::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_char_length_doublebyte));
}  

Long ex_function_oct_length::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_oct_length));
}  

Long ex_function_position::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_position));
}  

Long ex_function_position_doublebyte::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_position_doublebyte));
}  

Long ExFunctionTokenStr::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionTokenStr));
}  

Long ex_function_unixtime::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_unixtime));
}  

Long ex_function_sleep::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_sleep));
}  

Long ex_function_current::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_current));
}  

Long ex_function_encode::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_encode));
}

//++Triggers -
Long ex_function_unique_execute_id::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_unique_execute_id));
} 

Long ex_function_get_triggers_status::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_get_triggers_status));
} 

Long ex_function_get_bit_value_at::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_get_bit_value_at));
} 
//--Triggers -

Long ex_function_is_bitwise_and_true::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_is_bitwise_and_true));
} 
//--MV

Long ex_function_explode_varchar::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_explode_varchar));
}  
 
Long ex_function_hash::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_hash));
}  

Long ex_function_hivehash::pack(void * space_)
{
  Lng32 x = sizeof(ex_function_hivehash);
  return packClause(space_, sizeof(ex_function_hivehash));
}  

Long ExHashComb::pack(void * space_)
{
  return packClause(space_, sizeof(ExHashComb));
}

Long ExHiveHashComb::pack(void * space_)
{
  return packClause(space_, sizeof(ExHiveHashComb));
}

Long ExHDPHash::pack(void * space_)
{
  return packClause(space_, sizeof(ExHDPHash));
}  

Long ExHDPHashComb::pack(void * space_)
{
  return packClause(space_, sizeof(ExHDPHashComb));
}

Long ex_function_replace_null::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_replace_null));
}  

Long ex_function_mod::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_mod));
}  

Long ex_function_mask::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_mask));
}

Long ExFunctionShift::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionShift));
}  

Long ex_function_converttimestamp::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_converttimestamp));
}  

Long ex_function_dateformat::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_dateformat));
}  

Long ex_function_dayofweek::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_dayofweek));
}  

Long ex_function_extract::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_extract));
}  

Long ex_function_juliantimestamp::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_juliantimestamp));
}  

Long ex_function_exec_count::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_exec_count));
}  

Long ex_function_curr_transid::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_curr_transid));
}  

Long ex_function_ansi_user::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_ansi_user));
}

Long ex_function_user::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_user));
}  

Long ex_function_nullifzero::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_nullifzero));
}  

Long ex_function_nvl::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_nvl));
}  

Long ex_function_json_object_field_text::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_json_object_field_text));
}  


Long ex_function_queryid_extract::pack(void * space_)
{
  return packClause(space_, sizeof(ex_function_queryid_extract));
}  

Long ExFunctionUniqueId::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionUniqueId));
}  

Long ExFunctionRowNum::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionUniqueId));
}  

Long ExFunctionHbaseColumnLookup::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionHbaseColumnLookup));
}  

Long ExFunctionHbaseColumnsDisplay::pack(void * space_)
{
  colNames_.pack(space_);

  return packClause(space_, sizeof(ExFunctionHbaseColumnsDisplay));
}  

Long ExFunctionHbaseColumnCreate::pack(void * space_)
{
  return packClause(space_, sizeof(ExFunctionHbaseColumnCreate));
}  

Long ex_noop_clause::pack(void * space_)
{
  return packClause(space_, sizeof(ex_noop_clause));
}

Long ex_inout_clause::pack(void * space_)
{
  name.pack(space_);
  heading_.pack(space_);
  table_name_.pack(space_);
  schema_name_.pack(space_);
  catalog_name_.pack(space_);
  return packClause(space_, sizeof(ex_inout_clause));
}

Long ExFunctionSVariance::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionSVariance));
}  

Long ExFunctionSStddev::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionSStddev));
}  

Long ExpRaiseErrorFunction::pack(void * space)
{
  constraintName_.pack(space);
  tableName_.pack(space);
  return packClause(space, sizeof(ExpRaiseErrorFunction));
}  

Long ExFunctionRandomNum::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionRandomNum));
}  

Long ExFunctionRandomSelection::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionRandomSelection));
}  

Long ExProgDistrib::pack(void * space)
{
  return packClause(space, sizeof(ExProgDistrib));
}

Long ExProgDistribKey::pack(void * space)
{
  return packClause(space, sizeof(ExProgDistribKey));
}  

Long ExPAGroup::pack(void * space)
{
  return packClause(space, sizeof(ExPAGroup));
}

Long ExHash2Distrib::pack(void * space)
{
  return packClause(space, sizeof(ExHash2Distrib));
}

Long ExFunctionPack::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionPack));
}

Long ExFunctionRangeLookup::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionRangeLookup));
}

Long ExUnPackCol::pack(void * space)
{
  return packClause(space, sizeof(ExUnPackCol));
}
// MV,
Long ExFunctionGenericUpdateOutput::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionGenericUpdateOutput));
}  

//++ Triggers -
Long ExFunctionInternalTimestamp::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionInternalTimestamp));
}  

Long ExHeaderClause::pack(void * space)
{
  return packClause(space, sizeof(ExHeaderClause));
}  

Long ExFunctionSha::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionSha));
}

Long ExFunctionSha2::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionSha2));
}

Long ExFunctionMd5::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionMd5));
}

Long ExFunctionCrc32::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionCrc32));
}

Long ExFunctionIsIP::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionIsIP));
}

Long ExFunctionSoundex::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionSoundex));
}

Long ExFunctionInetAton::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionInetAton));
}

Long ExFunctionInetNtoa::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionInetNtoa));
}

Long ExFunctionAESEncrypt::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionAESEncrypt));
}

Long ExFunctionAESDecrypt::pack(void * space)
{
  return packClause(space, sizeof(ExFunctionAESDecrypt));
}
// -----------------------------------------------------------------------
// U N P A C K
// -----------------------------------------------------------------------

Lng32 ex_expr::unpack(void *base, void * reallocator)
{
  ExClausePtr currClausePtr;
  ex_clause *nextClause;
  ex_clause *currClause;

  if (clauses_.unpack(base, reallocator)) return -1;
  currClause = clauses_;
  if (currClause != NULL)
  {
     nextClause = clauses_->getNextClause();
     currClausePtr = nextClause;
  }
  else
     currClausePtr = (ExClausePtr)NULL;

  while (currClausePtr != (ExClausePtr)NULL)
  {
     if (currClausePtr.unpack(base, reallocator)) return -1;

     nextClause = currClausePtr.getPointer();
     currClause->setNextClause(nextClause);
     currClause = nextClause; 
     nextClause = currClause->getNextClause();
     currClausePtr = nextClause;
  }

  if (constantsArea_.unpack(base)) return -1;
  if (tempsArea_.unpack(base)) return -1;
  if (persistentArea_.unpack(base)) return -1;
  if (persistentInitializationArea_.unpack(base)) return -1;
  if (NOT generateNoPCode()) 
    if (pCode_.unpack(base, reallocator)) {return -1;}
  
  return NAVersionedObject::unpack(base, reallocator);
}

void ex_expr_lean::convOffsetToAddrInPCode(void* base) {
  pCodeLean_ = (PCodeBinary*)((char *)base - (Long)(pCodeLean_));

  PCodeBinary *pcode = getPCodeBinary();
  if (!pcode)
    return;

  // skip over the ATP's
  Int32 length = *(pcode++);
  pcode += (2 * length);

  while (pcode[0] != PCIT::END) {
    Int32 addrBuf[6];
    Int32 *addrs = PCode::getEmbeddedAddresses(pcode[0], addrBuf);
    for(Int32 i = 0; addrs[i] > 0; i++) {
#ifndef NA_LITTLE_ENDIAN
      //swapInt32((Int32*)(&(pcode[addrs[i]])));
#endif
      setPtrAsPCodeBinary(pcode, addrs[i], (Long)base -
                          GetPCodeBinaryAsPtr(pcode, addrs[i]));
    }
    pcode += PCode::getInstructionLength(pcode);
  }
  
}

Lng32 ex_expr_lean::unpack(void *base, void * reallocator)
{
  if (constantsArea_.unpack(base)) return -1;
  if (tempsArea_.unpack(base)) return -1;
  if (persistentArea_.unpack(base)) return -1;
  //  if (pCode_.unpack(base)) {return -1;}
  convOffsetToAddrInPCode(base);
  
  return NAVersionedObject::unpack(base, reallocator);
}

Lng32 AggrExpr::unpack(void *base, void * reallocator)
{
  if (initExpr_.unpack(base, reallocator)) return -1;
  if (perrecExpr_.unpack(base, reallocator)) return -1;
  if (finalNullExpr_.unpack(base, reallocator)) return -1;
  if (finalExpr_.unpack(base, reallocator)) return -1;
  if (groupingExpr_.unpack(base, reallocator)) return -1;

  return ex_expr::unpack(base, reallocator);
}


Lng32 ex_branch_clause::unpack(void *base, void * reallocator)
{
  if (branch_clause.unpack(base, reallocator)) return -1;
  if (saved_next_clause.unpack(base, reallocator)) return -1;
  return unpackClause(base, reallocator);
}

Lng32 ex_inout_clause::unpack(void *base, void * reallocator)
{
  if (name.unpack(base)) return -1;
  if (heading_.unpack(base)) return -1;
  if (table_name_.unpack(base)) return -1;
  if (schema_name_.unpack(base)) return -1;
  if (catalog_name_.unpack(base)) return -1;

  return unpackClause(base, reallocator);
}


Lng32 ExpRaiseErrorFunction::unpack(void *base, void * reallocator)
{
  if (constraintName_.unpack(base)) return -1;
  if (tableName_.unpack(base)) return -1;
  return unpackClause(base, reallocator);
}

Lng32 ExFunctionHbaseColumnCreate::unpack(void *base, void * reallocator)
{
  return unpackClause(base, reallocator);
}

Lng32 ExFunctionHbaseColumnsDisplay::unpack(void *base, void * reallocator)
{
  if (colNames_.unpack(base)) return -1;

  return unpackClause(base, reallocator);
}



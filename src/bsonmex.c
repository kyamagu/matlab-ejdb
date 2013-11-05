/** mxArray BSON encoder implementation.
 *
 * TODO: Implement sparse array conversion.
 *
 * Kota Yamaguchi 2013
 */

#include "bsonmex.h"
#include <ctype.h>
#include <mex.h>
#include <stdlib.h>
#include <string.h>

static bool ConvertArrayToBSON(const mxArray* input,
                               const char* name,
                               bson* output);
static mxArray* ConvertNextToMxArray(bson_iterator* it);
static mxArray* Convert2DOrNDArrayToCellArray(const mxArray* input);

/** Convert mxArray to BSON binary.
 */
static bool ConvertBinaryArrayToBSON(const mxArray* input,
                                     const char* name,
                                     bson* output) {
  size_t num_elements = mxGetNumberOfElements(input);
  uint8_t* values = (uint8_t*)mxGetData(input);
  if (num_elements == 0)
    return bson_append_null(output, (name) ? name : "0") == BSON_OK;
  const char* data = (const char*)mxGetData(input);
  int length = mxGetNumberOfElements(input);
  return bson_append_binary(output,
                            (name) ? name : "0",
                            BSON_BIN_BINARY,
                            data,
                            length) == BSON_OK;
}

/** Convert mxArray to BSON int array.
 */
static bool ConvertShortArrayToBSON(const mxArray* input,
                                    const char* name,
                                    bson* output) {
  char key[16];
  size_t num_elements = mxGetNumberOfElements(input);
  int16_t* values = (int16_t*)mxGetData(input);
  if (num_elements == 0)
    return bson_append_null(output, (name) ? name : "0") == BSON_OK;
  if (num_elements == 1)
    return bson_append_int(output, (name) ? name : "0", values[0]) ==
           BSON_OK;
  if (name && bson_append_start_array(output, name) != BSON_OK)
    return false;
  for (int i = 0; i < num_elements; ++i) {
    if (sprintf(key, "%d", i) < 0)
      return false;
    if (bson_append_int(output, key, values[i]) != BSON_OK)
      return false;
  }
  if (name && bson_append_finish_array(output) != BSON_OK)
    return false;
  return true;
}

/** Convert mxArray to BSON int array.
 */
static bool ConvertIntegerArrayToBSON(const mxArray* input,
                                      const char* name,
                                      bson* output) {
  char key[16];
  size_t num_elements = mxGetNumberOfElements(input);
  int32_t* values = (int32_t*)mxGetData(input);
  if (num_elements == 0)
    return bson_append_null(output, (name) ? name : "0") == BSON_OK;
  if (num_elements == 1)
    return bson_append_int(output, (name) ? name : "0", values[0]) ==
           BSON_OK;
  if (name && bson_append_start_array(output, name) != BSON_OK)
    return false;
  for (int i = 0; i < num_elements; ++i) {
    if (sprintf(key, "%d", i) < 0)
      return false;
    if (bson_append_int(output, key, values[i]) != BSON_OK)
      return false;
  }
  if (name && bson_append_finish_array(output) != BSON_OK)
    return false;
  return true;
}

/** Convert mxArray to BSON long array.
 */
static bool ConvertLongArrayToBSON(const mxArray* input,
                                   const char* name,
                                   bson* output) {
  char key[16];
  size_t num_elements = mxGetNumberOfElements(input);
  int64_t* values = (int64_t*)mxGetData(input);
  if (num_elements == 0)
    return bson_append_null(output, (name) ? name : "0") == BSON_OK;
  if (num_elements == 1)
    return bson_append_long(output, (name) ? name : "0", values[0]) ==
           BSON_OK;
  if (name && bson_append_start_array(output, name) != BSON_OK)
    return false;
  for (int i = 0; i < num_elements; ++i) {
    if (sprintf(key, "%d", i) < 0)
      return false;
    if (bson_append_long(output, key, values[i]) != BSON_OK)
      return false;
  }
  if (name && bson_append_finish_array(output) != BSON_OK)
    return false;
  return true;
}

/** Convert mxArray to BSON bool array.
 */
static bool ConvertLogicalArrayToBSON(const mxArray* input,
                                      const char* name,
                                      bson* output) {
  char key[16];
  size_t num_elements = mxGetNumberOfElements(input);
  mxLogical* values = mxGetLogicals(input);
  if (num_elements == 0)
    return bson_append_null(output, (name) ? name : "0") == BSON_OK;
  if (num_elements == 1)
    return bson_append_bool(output, (name) ? name : "0", values[0]) ==
           BSON_OK;
  if (name && bson_append_start_array(output, name) != BSON_OK)
    return false;
  for (int i = 0; i < num_elements; ++i) {
    if (sprintf(key, "%d", i) < 0)
      return false;
    if (bson_append_bool(output, key, values[i]) != BSON_OK)
      return false;
  }
  if (name && bson_append_finish_array(output) != BSON_OK)
    return false;
  return true;
}

/** Convert mxArray to BSON string.
 */
static bool ConvertCharArrayToBSON(const mxArray* input,
                                   const char* name,
                                   bson* output) {
  char* value = mxArrayToString(input);
  if (!value)
    return false;
  bool status = bson_append_string(output, (name) ? name : "0", value) == BSON_OK;
  mxFree(value);
  return status;
}

/** Convert mxArray to BSON double array.
 */
static bool ConvertFloatArrayToBSON(const mxArray* input,
                                    const char* name,
                                    bson* output) {
  char key[16];
  size_t num_elements = mxGetNumberOfElements(input);
  float* values = (float*)mxGetData(input);
  if (num_elements == 0)
    return bson_append_null(output, (name) ? name : "0") == BSON_OK;
  if (num_elements == 1)
    return bson_append_double(output, (name) ? name : "0", values[0]) ==
           BSON_OK;
  if (name && bson_append_start_array(output, name) != BSON_OK)
    return false;
  for (int i = 0; i < num_elements; ++i) {
    if (sprintf(key, "%d", i) < 0)
      return false;
    if (bson_append_double(output, key, values[i]) != BSON_OK)
      return false;
  }
  if (name && bson_append_finish_array(output) != BSON_OK)
    return false;
  return true;
}

/** Convert mxArray to BSON double array.
 */
static bool ConvertDoubleArrayToBSON(const mxArray* input,
                                     const char* name,
                                     bson* output) {
  char key[16];
  size_t num_elements = mxGetNumberOfElements(input);
  double* values = mxGetPr(input);
  if (num_elements == 0)
    return bson_append_null(output, (name) ? name : "0") == BSON_OK;
  if (num_elements == 1)
    return bson_append_double(output, (name) ? name : "0", values[0]) ==
           BSON_OK;
  if (name && bson_append_start_array(output, name) != BSON_OK)
    return false;
  for (int i = 0; i < num_elements; ++i) {
    if (sprintf(key, "%d", i) < 0)
      return false;
    if (bson_append_double(output, key, values[i]) != BSON_OK)
      return false;
  }
  if (name && bson_append_finish_array(output) != BSON_OK)
    return false;
  return true;
}

/** Convert mxArray to BSON date array.
 */
static bool ConvertDateArrayToBSON(const mxArray* input,
                                   const char* name,
                                   bson* output) {
  char key[16];
  size_t num_elements = mxGetNumberOfElements(input);
  if (num_elements == 0)
    return bson_append_null(output, (name) ? name : "0") == BSON_OK;
  if (num_elements == 1) {
    mxArray* value = mxGetProperty(input, 0, "number");
    if (!value)
      return false;
    bson_date_t date_value = (bson_date_t)(
        (mxGetScalar(value) - 719529) * 86400);
    return bson_append_date(output, (name) ? name : "0", date_value) ==
           BSON_OK;
  }
  if (name && bson_append_start_array(output, name) != BSON_OK)
    return false;
  for (int i = 0; i < num_elements; ++i) {
    if (sprintf(key, "%d", i) < 0)
      return false;
    mxArray* value = mxGetProperty(input, i, "number");
    if (!value)
      return false;
    bson_date_t date_value = (bson_date_t)(
      (mxGetScalar(value) - 719529) * 86400);
    if (bson_append_date(output, key, date_value) != BSON_OK)
      return false;
  }
  if (name && bson_append_finish_array(output) != BSON_OK)
    return false;
  return true;
}

/** Return true if the input is a query cell array.
 */
static bool CheckQueryCellArray(const mxArray* input) {
  size_t num_elements = mxGetNumberOfElements(input);
  if (mxGetNumberOfElements(input) % 2 != 0)
    return false;
  bool any_operator_key = false;
  for (int i = 0; i < num_elements; i += 2) {
    mxArray* element = mxGetCell(input, i);
    any_operator_key |= mxIsChar(element) &&
                        mxGetNumberOfElements(element) > 0;
                        // mxGetChars(element)[0] == '$';
  }
  return any_operator_key;
}

/** Convert cell mxArray to BSON array.
 */
static bool ConvertCellArrayToBSON(const mxArray* input,
                                   const char* name,
                                   bson* output) {
  char key[16];
  size_t num_elements = mxGetNumberOfElements(input);
  if (name && bson_append_start_array(output, name) != BSON_OK)
    return false;
  if (output->flags & BSON_FLAG_QUERY_MODE && CheckQueryCellArray(input))
    for (int i = 0; i < num_elements; i += 2) {
      char* key = mxArrayToString(mxGetCell(input, i));
      if (!key)
        return false;
      mxArray* value = mxGetCell(input, i + 1);
      if (!ConvertArrayToBSON(value, key, output))
        return false;
      mxFree(key);
    }
  else
    for (int i = 0; i < num_elements; ++i) {
      mxArray* element = mxGetCell(input, i);
      if (sprintf(key, "%d", i) < 0)
        return false;
      if (!ConvertArrayToBSON(element, key, output))
        return false;
    }
  if (name && bson_append_finish_array(output) != BSON_OK)
    return false;
  return true;
}

/** Check if oid is given.
 */
static bool ConvertStringToOID(const mxArray* element,
                               bson* output) {
  char* value = mxArrayToString(element);
  bson_oid_t oid;
  bson_oid_from_string(&oid, value);
  mxFree(value);
  return bson_append_oid(output, "_id", &oid) == BSON_OK;
}

/** Convert struct mxArray to BSON array.
 */
static bool ConvertStructArrayToBSON(const mxArray* input,
                                     const char* name,
                                     bson* output) {
  size_t num_elements = mxGetNumberOfElements(input);
  int num_fields = mxGetNumberOfFields(input);
  if (num_elements == 1) {
    if (name && bson_append_start_object(output, name) != BSON_OK)
      return false;
    for (int i = 0; i < num_fields; ++i) {
      mxArray* element = mxGetFieldByNumber(input, 0, i);
      const char* field_name = mxGetFieldNameByNumber(input, i);
      // Convert string to OID only if a scalar struct with id field.
      if (name == NULL &&
          strcmp(field_name, "id_") == 0 &&
          mxIsChar(element) &&
          mxGetNumberOfElements(element) == 12) {
        if (!ConvertStringToOID(element, output))
          return false;
      }
      else
        if (!ConvertArrayToBSON(element, field_name, output))
          return false;
    }
    if (name && bson_append_finish_object(output) != BSON_OK)
      return false;
  }
  else {
    char key[16];
    if (name && bson_append_start_array(output, name) != BSON_OK)
      return false;
    for (int j = 0; j < num_elements; ++j) {
      if (sprintf(key, "%d", j) < 0)
        return false;
      if (bson_append_start_object(output, key) != BSON_OK)
        return false;
      for (int i = 0; i < num_fields; ++i) {
        mxArray* element = mxGetFieldByNumber(input, 0, i);
        const char* field_name = mxGetFieldNameByNumber(input, i);
        if (!ConvertArrayToBSON(element, field_name, output))
          return false;
      }
      if (bson_append_finish_object(output) != BSON_OK)
        return false;
    }
    if (name && bson_append_finish_array(output) != BSON_OK)
      return false;
  }
  return true;
}

/** Convert any ND (>2D) array to a nested cell array.
 */
static mxArray* ConvertNDArrayToCellArray(const mxArray* input,
                                   mwSize ndims,
                                   const mwSize* dims) {
  mwSize last_dimentions = dims[ndims - 1];
  mxArray* array = mxCreateCellMatrix(1, last_dimentions);
  mwSize num_elements = mxGetNumberOfElements(input) / last_dimentions;
  for (int i = 0; i < last_dimentions; ++i) {
    mxArray* element = NULL;
    switch (mxGetClassID(input)) {
      case mxSTRUCT_CLASS: {
        int nfields = mxGetNumberOfFields(input);
        const char** fields = (const char**)mxMalloc(
            sizeof(const char*) * nfields);
        for (int k = 0; k < nfields; ++k)
          fields[k] = mxGetFieldNameByNumber(input, k);
        element = mxCreateStructArray(ndims - 1, dims, nfields, fields);
        for (int j = 0; j < num_elements; ++j) {
          mwSize index = j + i * num_elements;
          for (int k = 0; k < nfields; ++k) {
            mxArray* value = mxDuplicateArray(mxGetFieldByNumber(input,
                                                                 index,
                                                                 k));
            mxSetFieldByNumber(element, j, k, value);
          }
        }
        mxFree(fields);
        break;
      }
      case mxCELL_CLASS: {
        element = mxCreateCellArray(ndims - 1, dims);
        for (int j = 0; j < num_elements; ++j) {
          mwSize index = j + num_elements * i;
          mxSetCell(element, j, mxDuplicateArray(mxGetCell(input, index)));
        }
        break;
      }
      case mxDOUBLE_CLASS:
      case mxINT8_CLASS:
      case mxUINT8_CLASS:
      case mxINT16_CLASS:
      case mxUINT16_CLASS:
      case mxINT32_CLASS:
      case mxUINT32_CLASS:
      case mxINT64_CLASS:
      case mxUINT64_CLASS:
      case mxSINGLE_CLASS: {
        element = mxCreateNumericArray(ndims - 1,
                                       dims,
                                       mxGetClassID(input),
                                       mxREAL);
        mwSize element_size = mxGetElementSize(input);
        mwSize stride = element_size * num_elements;
        memcpy((void*)mxGetData(element),
               (void*)mxGetData(input) + i * stride,
               stride);
        break;
      }
      case mxCHAR_CLASS: {
        element = mxCreateCharArray(ndims - 1, dims);
        mwSize element_size = mxGetElementSize(input);
        mwSize stride = element_size * num_elements;
        memcpy((void*)mxGetData(element),
               (void*)mxGetData(input) + i * stride,
               stride);
        break;
      }
      case mxLOGICAL_CLASS: {
        element = mxCreateLogicalArray(ndims - 1, dims);
        mwSize element_size = mxGetElementSize(input);
        mwSize stride = element_size * num_elements;
        memcpy((void*)mxGetData(element),
               (void*)mxGetData(input) + i * stride,
               stride);
        break;
      }
      case mxOBJECT_CLASS:
      case mxVOID_CLASS:
      case mxFUNCTION_CLASS:
      case mxOPAQUE_CLASS:
      default:
        return NULL;
    }
    if (!element)
      return NULL;
    mxArray* split_element = Convert2DOrNDArrayToCellArray(element);
    if (!split_element)
      return NULL;
    if (split_element != element)
      mxDestroyArray(element);
    mxSetCell(array, i, split_element);
  }
  return array;
}

/** Convert any 2D array to a cell array of row vectors.
 */
static mxArray* Convert2DArrayToCellArray(const mxArray* input,
                                   mwSize ndims,
                                   const mwSize* dims) {
  mxArray* array = mxCreateCellMatrix(1, dims[0]);
  for (int i = 0; i < dims[0]; ++i) {
    mxArray* element = NULL;
    switch (mxGetClassID(input)) {
      case mxSTRUCT_CLASS: {
        int nfields = mxGetNumberOfFields(input);
        const char** fields = (const char**)mxMalloc(
            sizeof(const char*) * nfields);
        for (int k = 0; k < nfields; ++k)
          fields[k] = mxGetFieldNameByNumber(input, k);
        element = mxCreateStructMatrix(1, dims[1], nfields, fields);
        for (int j = 0; j < dims[1]; ++j) {
          mwSize index = i + j * dims[0];
          for (int k = 0; k < nfields; ++k) {
            mxArray* value = mxDuplicateArray(mxGetFieldByNumber(input,
                                                                 index,
                                                                 k));
            mxSetFieldByNumber(element, j, k, value);
          }
        }
        mxFree(fields);
        break;
      }
      case mxCELL_CLASS: {
        element = mxCreateCellMatrix(1, dims[1]);
        for (int j = 0; j < dims[1]; ++j) {
          mwSize index = i + j * dims[0];
          mxSetCell(element, j, mxDuplicateArray(mxGetCell(input, index)));
        }
        break;
      }
      case mxDOUBLE_CLASS:
      case mxINT8_CLASS:
      case mxUINT8_CLASS:
      case mxINT16_CLASS:
      case mxUINT16_CLASS:
      case mxINT32_CLASS:
      case mxUINT32_CLASS:
      case mxINT64_CLASS:
      case mxUINT64_CLASS:
      case mxSINGLE_CLASS: {
        element = mxCreateNumericMatrix(1,
                                        dims[1],
                                        mxGetClassID(input),
                                        mxREAL);
        mwSize element_size = mxGetElementSize(input);
        mwSize stride = element_size * dims[0];
        void* input_data = (void*)mxGetData(input) + i * element_size;
        void* output_data = (void*)mxGetData(element);
        for (int j = 0; j < dims[1]; ++j) {
          memcpy(output_data, input_data, element_size);
          input_data += stride;
          output_data += element_size;
        }
        break;
      }
      case mxCHAR_CLASS: {
        mwSize char_dims[] = {1, dims[1]};
        element = mxCreateCharArray(2, char_dims);
        mwSize element_size = mxGetElementSize(input);
        mwSize stride = element_size * dims[0];
        void* input_data = (void*)mxGetData(input) + i * element_size;
        void* output_data = (void*)mxGetData(element);
        for (int j = 0; j < dims[1]; ++j) {
          memcpy(output_data, input_data, element_size);
          input_data += stride;
          output_data += element_size;
        }
        break;
      }
      case mxLOGICAL_CLASS: {
        element = mxCreateLogicalMatrix(1, dims[1]);
        mwSize element_size = mxGetElementSize(input);
        mwSize stride = element_size * dims[0];
        void* input_data = (void*)mxGetData(input) + i * element_size;
        void* output_data = (void*)mxGetData(element);
        for (int j = 0; j < dims[1]; ++j) {
          memcpy(output_data, input_data, element_size);
          input_data += stride;
          output_data += element_size;
        }
        break;
      }
      case mxOBJECT_CLASS:
      case mxVOID_CLASS:
      case mxFUNCTION_CLASS:
      case mxOPAQUE_CLASS:
      default:
        return NULL;
    }
    if (!element)
      return NULL;
    mxSetCell(array, i, element);
  }
  return array;
}

/** Convert any 2D or ND array to a nested cell array.
 */
static mxArray* Convert2DOrNDArrayToCellArray(const mxArray* input) {
  mwSize ndims = mxGetNumberOfDimensions(input);
  const mwSize* dims = mxGetDimensions(input);
  if (ndims <= 2 && (dims[0] <= 1 || dims[1] <= 1)) {
    return (mxArray*)input;
  }
  else if (ndims == 2)
    return Convert2DArrayToCellArray(input, ndims, dims);
  else
    return ConvertNDArrayToCellArray(input, ndims, dims);
}

/** Convert any mxArray to BSON.
 */
static bool ConvertArrayToBSON(const mxArray* input,
                               const char* name,
                               bson* output) {
  mxArray* array = Convert2DOrNDArrayToCellArray(input);
  if (!array)
    return false;
  switch (mxGetClassID(array)) {
    case mxDOUBLE_CLASS:
      return ConvertDoubleArrayToBSON(array, name, output);
      break;
    case mxSTRUCT_CLASS:
      return ConvertStructArrayToBSON(array, name, output);
      break;
    case mxCELL_CLASS:
      return ConvertCellArrayToBSON(array, name, output);
      break;
    case mxLOGICAL_CLASS:
      return ConvertLogicalArrayToBSON(array, name, output);
      break;
    case mxCHAR_CLASS:
      return ConvertCharArrayToBSON(array, name, output);
      break;
    case mxINT8_CLASS:
    case mxUINT8_CLASS:
      return ConvertBinaryArrayToBSON(array, name, output);
      break;
    case mxINT16_CLASS:
    case mxUINT16_CLASS:
      return ConvertShortArrayToBSON(array, name, output);
      break;
    case mxINT32_CLASS:
    case mxUINT32_CLASS:
      return ConvertIntegerArrayToBSON(array, name, output);
      break;
    case mxINT64_CLASS:
    case mxUINT64_CLASS:
      return ConvertLongArrayToBSON(array, name, output);
      break;
    case mxSINGLE_CLASS:
      return ConvertFloatArrayToBSON(array, name, output);
      break;
    case mxOBJECT_CLASS:
    case mxVOID_CLASS:
    case mxFUNCTION_CLASS:
    case mxOPAQUE_CLASS:
    default:
      if (mxIsClass(input, "bson.date")) {
        return ConvertDateArrayToBSON(array, name, output);
        break;        
      }
      return false;
  }
  if (array != input)
    mxDestroyArray(array);
  return false;
}

/** Check the type and the size of the BSON object.
 */
static void ScanBSONObject(bson_iterator* it,
                           int* object_size, 
                           const char*** keys,
                           int* array_type) {
  int element_type;
  bool is_first = true;
  bool is_array = true;
  *object_size = 0;
  while (bson_iterator_more(it)) {
    bson_type type = bson_iterator_next(it);
    if (type == BSON_EOO)
      break;
    const char* key = bson_iterator_key(it);
    (*object_size)++;
    // Keep key names for a struct array.
    if (keys) {
      *keys = (const char**)mxRealloc(*keys,
                                      *object_size * sizeof(const char*));
      (*keys)[(*object_size) - 1] = key;
    }
    // Check if it has an consistent index.
    const char* key_ptr = key;
    bool is_digit = true;
    while (*key_ptr != 0)
      is_digit &= isdigit(*key_ptr++);
    is_array = (is_digit) ? is_array & (*object_size - 1 == atol(key)) : false;
    // Check the array type from element.
    if (array_type) {
      switch (type) {
        case BSON_DOUBLE:
          element_type = mxDOUBLE_CLASS;
          break;
        case BSON_INT:
          element_type = mxINT32_CLASS;
          break;
        case BSON_LONG:
          element_type = mxINT64_CLASS;
          break;
        case BSON_BOOL:
          element_type = mxLOGICAL_CLASS;
          break;
        case BSON_STRING:
          element_type = mxCHAR_CLASS;
          break;
        case BSON_BINDATA:
          element_type = mxUINT8_CLASS;
          break;
        default:
          element_type = mxCELL_CLASS;
          break;
      }
      if (is_first) {
        *array_type = element_type;
        is_first = false;
      }
      else
        *array_type = (*array_type == element_type) ?
                      element_type : mxCELL_CLASS;
    }
  }
  if (array_type && !is_array)
    *array_type = mxSTRUCT_CLASS;
}

/** Convert BSON array to double mxArray.
 */
static mxArray* ConvertBSONArrayToDoubleArray(bson_iterator* it, int size) {
  mxArray* element = mxCreateDoubleMatrix(1, size, mxREAL);
  if (!element)
    return NULL;
  double* output_data = mxGetPr(element);
  while (bson_iterator_more(it)) {
    bson_type type = bson_iterator_next(it);
    switch (type) {
      case BSON_EOO:
        break;
      case BSON_DOUBLE:
        *(output_data++) = bson_iterator_double(it);
        break;
      case BSON_INT:
        *(output_data++) = bson_iterator_int(it);
        break;
      case BSON_LONG:
        *(output_data++) = bson_iterator_long(it);
        break;
      case BSON_BOOL:
        *(output_data++) = bson_iterator_bool(it);
        break;
      default:
        mxDestroyArray(element);
        return NULL;
    }
  }
  return element;
}

/** Convert BSON array to integer mxArray.
 */
static mxArray* ConvertBSONArrayToIntegerArray(bson_iterator* it, int size) {
  mxArray* element = mxCreateNumericMatrix(1, size, mxINT32_CLASS, mxREAL);
  if (!element)
    return NULL;
  int32_t* output_data = (int32_t*)mxGetData(element);
  while (bson_iterator_more(it)) {
    bson_type type = bson_iterator_next(it);
    switch (type) {
      case BSON_EOO:
        break;
      case BSON_DOUBLE:
        *(output_data++) = bson_iterator_double(it);
        break;
      case BSON_INT:
        *(output_data++) = bson_iterator_int(it);
        break;
      case BSON_LONG:
        *(output_data++) = bson_iterator_long(it);
        break;
      case BSON_BOOL:
        *(output_data++) = bson_iterator_bool(it);
        break;
      default:
        mxDestroyArray(element);
        return NULL;
    }
  }
  return element;
}

/** Convert BSON array to long mxArray.
 */
static mxArray* ConvertBSONArrayToLongArray(bson_iterator* it, int size) {
  mxArray* element = mxCreateNumericMatrix(1, size, mxINT64_CLASS, mxREAL);
  if (!element)
    return NULL;
  int64_t* output_data = (int64_t*)mxGetData(element);
  while (bson_iterator_more(it)) {
    bson_type type = bson_iterator_next(it);
    switch (type) {
      case BSON_EOO:
        break;
      case BSON_DOUBLE:
        *(output_data++) = bson_iterator_double(it);
        break;
      case BSON_INT:
        *(output_data++) = bson_iterator_int(it);
        break;
      case BSON_LONG:
        *(output_data++) = bson_iterator_long(it);
        break;
      case BSON_BOOL:
        *(output_data++) = bson_iterator_bool(it);
        break;
      default:
        mxDestroyArray(element);
        return NULL;
    }
  }
  return element;
}

/** Convert BSON array to logical mxArray.
 */
static mxArray* ConvertBSONArrayToLogicalArray(bson_iterator* it, int size) {
  mxArray* element = mxCreateLogicalMatrix(1, size);
  if (!element)
    return NULL;
  mxLogical* output_data = mxGetLogicals(element);
  while (bson_iterator_more(it)) {
    bson_type type = bson_iterator_next(it);
    switch (type) {
      case BSON_EOO:
        break;
      case BSON_DOUBLE:
        *(output_data++) = bson_iterator_double(it);
        break;
      case BSON_INT:
        *(output_data++) = bson_iterator_int(it);
        break;
      case BSON_LONG:
        *(output_data++) = bson_iterator_int(it);
        break;
      case BSON_BOOL:
        *(output_data++) = bson_iterator_bool(it);
        break;
      default:
        mxDestroyArray(element);
        return NULL;
    }
  }
  return element;
}

/** Convert BSON array to cell mxArray.
 */
static mxArray* ConvertBSONArrayToCellArray(bson_iterator* it, int size) {
  mxArray* element = mxCreateCellMatrix(1, size);
  if (!element)
    return NULL;
  int index = 0;
  bool is_end = false;
  for (int i = 0; i < size; ++i) {
    mxArray* sub_element = ConvertNextToMxArray(it);
    if (!sub_element) {
      mxDestroyArray(element);
      return NULL;
    }
    mxSetCell(element, i, sub_element);
  }
  return element;
}

/** Convert keys to matlab-safe names.
 */
static char** CreateSafeKeys(int size, const char* keys[]) {
  char buffer[64]; // Matlab's variable can be up to 63 characters.
  char** safe_keys = (char**)mxMalloc(size * sizeof(char*));
  for (int i = 0; i < size; ++i) {
    const char* input = keys[i];
    char* output = buffer;
    memset(buffer, 0, 64 * sizeof(char));
    // Special case: oid maps to "id_" field.
    if (strcmp(input, "_id") == 0) {
      strcpy(output, "id_");
      output += strlen(output);
    }
    else {
      // variable name: [a-zA-Z][a-zA-Z0-9_]*
      // Trim leading non-alphanumeric.
      while (*input && !isalnum(*input))
        ++input;
      // If starting from numeric, prepend 'x'.
      if (*input && isdigit(*input))
        *output++ = 'x';
      while (*input && output < &buffer[63]) {
        if (isalnum(*input))
          *output++ = *input++;
        else {
          // Convert any consecutive non-alphanumeric to underscore.
          *output++ = '_';
          while (*input && !isalnum(*input))
            ++input;
        }
      }
    }
    // If empty, name it 'x'.
    if (output == buffer)
      *output++ = 'x';
    // Check if the name is duplicated. If it is, append a number.
    bool duplicated;
    int suffix = 0;
    do {
      duplicated = false;
      for (int j = 0; j < i; ++j)
        duplicated |= (strcmp(safe_keys[j], buffer) == 0);
      if (duplicated) {
        // Append a suffix if duplicated.
        char suffix_buffer[32];
        int length = sprintf(suffix_buffer, "%d", suffix++);
        if (output + length < &buffer[31])
          strcpy(output, suffix_buffer);
        else {
          // No resolution to the name collision...
          for (int j = 0; j < i; ++j)
            mxFree(safe_keys[j]);
          mxFree(safe_keys);
          return NULL;
        }
      }
    } while (duplicated);
    // Copy the safe key name.
    safe_keys[i] = (char*)mxCalloc(strlen(buffer)+1, sizeof(char));
    strcpy(safe_keys[i], buffer);
  }
  return safe_keys;
}

/** Delete keys.
 */
static void DestroySafekeys(int size, char** keys) {
  for (int i = 0; i < size; ++i)
    mxFree(keys[i]);
  mxFree(keys);
}

/** Convert BSON array to struct mxArray.
 */
static mxArray* ConvertBSONArrayToStructArray(bson_iterator* it,
                                              int size,
                                              const char** keys) {
  char** safe_keys = CreateSafeKeys(size, keys);
  if (!safe_keys)
    return NULL;
  mxArray* element = mxCreateStructMatrix(1,
                                          1,
                                          size,
                                          (const char**)safe_keys);
  DestroySafekeys(size, safe_keys);
  if (!element)
    return NULL;
  int index = 0;
  for (int i = 0; i < size; ++i) {
    mxArray* sub_element = ConvertNextToMxArray(it);
    if (!sub_element) {
      mxDestroyArray(element);
      return NULL;
    }
    mxSetFieldByNumber(element, 0, i, sub_element);
  }
  return element;
}

/** Merge cell array of numeric arrays to an N-D numeric array.
 */
static void MergeNumericArrays(mxArray** array) {
  int size = mxGetNumberOfElements(*array);
  mxArray* element = mxGetCell(*array, 0);
  mxClassID class_id = mxGetClassID(element);
  mwSize ndims = mxGetNumberOfDimensions(element);
  const mwSize* dims = mxGetDimensions(element);
  if (ndims < 2)
    return;
  mxArray* new_array = NULL;
  if (dims[0] == 1) {
    // Stack row vectors.
    new_array = mxCreateNumericMatrix(size, dims[1], class_id, mxREAL);
    if (!new_array)
      return;
    size_t value_size = mxGetNumberOfElements(element);
    size_t element_size = mxGetElementSize(element);
    for (int i = 0; i < size; ++i) {
      mxArray* value = mxGetCell(*array, i);
      void* input = (void*)mxGetData(value);
      void* output = (void*)mxGetData(new_array) + i * element_size;
      for (int j = 0; j < value_size; ++j) {
        memcpy(output, input, element_size);
        input += element_size;
        output += size * element_size;
      }
    }
  }
  else {
    // Expand the last dimension.
    mwSize* new_dims = (mwSize*)mxMalloc((ndims + 1) * sizeof(mwSize));
    memcpy(new_dims, dims, ndims * sizeof(mwSize));
    new_dims[ndims] = size;
    new_array = mxCreateNumericArray(ndims + 1, new_dims, class_id, mxREAL);
    mxFree(new_dims);
    if (!new_array)
      return;
    size_t stride = mxGetNumberOfElements(element) * mxGetElementSize(element);
    for (int i = 0; i < size; ++i) {
      mxArray* value = mxGetCell(*array, i);
      memcpy(mxGetData(new_array) + i * stride, mxGetData(value), stride);
    }
  }
  mxDestroyArray(*array);
  *array = new_array;
}

/** Merge cell array of cell arrays to an N-D cell array.
 */
static void MergeCellArrays(mxArray** array) {
  int size = mxGetNumberOfElements(*array);
  mxArray* element = mxGetCell(*array, 0);
  mxClassID class_id = mxGetClassID(element);
  mwSize ndims = mxGetNumberOfDimensions(element);
  const mwSize* dims = mxGetDimensions(element);
  if (ndims < 2)
    return;
  mxArray* new_array = NULL;
  if (ndims == 2 && dims[0] == 1) {
    // Stack row vectors.
    new_array = mxCreateCellMatrix(size, dims[1]);
    if (!new_array)
      return;
    for (int i = 0; i < size; ++i) {
      mxArray* value = mxGetCell(*array, i);
      for (int j = 0; j < dims[1]; ++j)
        mxSetCell(new_array,
                  i + j * size,
                  mxDuplicateArray(mxGetCell(value, j)));
    }
  }
  else {
    // Expand the last dimension.
    mwSize* new_dims = (mwSize*)mxMalloc(sizeof(mwSize) * (ndims + 1));
    memcpy(new_dims, dims, sizeof(mwSize) * ndims);
    new_dims[ndims] = size;
    new_array = mxCreateCellArray(ndims + 1, new_dims);
    mxFree(new_dims);
    mwSize element_size = mxGetNumberOfElements(element);
    for (int i = 0; i < size; ++i) {
      mxArray* value = mxGetCell(*array, i);
      for (int j = 0; j < element_size; ++j) {
        mxSetCell(new_array,
                  j + i * element_size,
                  mxDuplicateArray(mxGetCell(value, j)));
      }
    }
  }
  mxDestroyArray(*array);
  *array = new_array;
}

/** Merge cell array of struct arrays to an N-D cell array.
 */
static void MergeStructArrays(mxArray** array) {
  int size = mxGetNumberOfElements(*array);
  mxArray* element = mxGetCell(*array, 0);
  mxClassID class_id = mxGetClassID(element);
  mwSize ndims = mxGetNumberOfDimensions(element);
  const mwSize* dims = mxGetDimensions(element);
  if (ndims < 2)
    return;
  mxArray* new_array = NULL;
  // Check if all fields are the same.
  int num_fields = mxGetNumberOfFields(element);
  const char** fields = (const char**)mxMalloc(
      sizeof(const char*) * num_fields);
  for (int i = 0; i < num_fields; ++i)
    fields[i] = mxGetFieldNameByNumber(element, i);
  bool mergeable = true;
  for (int i = 1; i < size; ++i) {
    mxArray* value = mxGetCell(*array, i);
    if (mxGetNumberOfFields(value) == num_fields)
      for (int k = 0; k < num_fields; ++k)
        mergeable &= strcmp(fields[k], mxGetFieldNameByNumber(value, k)) == 0;
    else
      mergeable = false;
  }
  if (!mergeable) {
    mxFree(fields);
    return;
  }
  if (ndims == 2 && dims[0] == 1 && dims[1] == 1) {
    // Concatenate into a row vector.
    new_array = mxCreateStructMatrix(1, size, num_fields, fields);
    if (!new_array)
      return;
    for (int i = 0; i < size; ++i) {
      mxArray* value = mxGetCell(*array, i);
      for (int k = 0; k < num_fields; ++k) {
        mxArray* field = mxDuplicateArray(mxGetFieldByNumber(value, 0, k));
        mxSetFieldByNumber(new_array, i, k, field);
      }
    }
  }
  else if (ndims == 2 && dims[0] == 1) {
    // Stack row vectors.
    new_array = mxCreateStructMatrix(size, dims[1], num_fields, fields);
    if (!new_array)
      return;
    for (int i = 0; i < size; ++i) {
      mxArray* value = mxGetCell(*array, i);
      for (int j = 0; j < dims[1]; ++j)
        for (int k = 0; k < num_fields; ++k) {
          mxArray* field = mxDuplicateArray(mxGetFieldByNumber(value, j, k));
          mxSetFieldByNumber(new_array, i + j * size, k, field);
        }
    }
  }
  else {
    // Expand the last dimension.
    mwSize* new_dims = (mwSize*)mxMalloc(sizeof(mwSize) * (ndims + 1));
    memcpy(new_dims, dims, sizeof(mwSize) * ndims);
    new_dims[ndims] = size;
    new_array = mxCreateStructArray(ndims + 1, new_dims, num_fields, fields);
    mxFree(new_dims);
    mwSize element_size = mxGetNumberOfElements(element);
    for (int i = 0; i < size; ++i) {
      mxArray* value = mxGetCell(*array, i);
      for (int j = 0; j < element_size; ++j)
        for (int k = 0; k < num_fields; ++k) {
          mxArray* field = mxDuplicateArray(mxGetFieldByNumber(value, j, k));
          mxSetFieldByNumber(new_array, j + i * element_size, k, field);
        }
    }
  }
  mxFree(fields);
  mxDestroyArray(*array);
  *array = new_array;
}

/** Merge cell array of bson.date arrays to an N-D date array.
 */
static void MergeDateArrays(mxArray** array) {
  int size = mxGetNumberOfElements(*array);
  mxArray* element = mxGetCell(*array, 0);
  mxClassID class_id = mxGetClassID(element);
  mwSize ndims = mxGetNumberOfDimensions(element);
  const mwSize* dims = mxGetDimensions(element);
  if (ndims < 2)
    return;
  mxArray* new_array = NULL;
  int dimension = (ndims == 2 && dims[0] == 1 && dims[1] == 1) ? 2 :
                  (ndims == 2 && dims[0] == 1) ? 1 : ndims;
  mxArray** rhs = (mxArray**)mxMalloc(sizeof(mxArray*) * (1 + size));
  rhs[0] = mxCreateDoubleScalar(2);
  for (int i = 0; i < size; ++i)
    rhs[i + 1] = mxGetCell(*array, i);
  mexCallMATLAB(1, &new_array, (1 + size), rhs, "cat");
  mxDestroyArray(rhs[0]);
  mxFree(rhs);
  mxDestroyArray(*array);
  *array = new_array;
}

void TryMergeCellToNDArray(mxArray** array) {
  int size = mxGetNumberOfElements(*array);
  if (!size)
    return;
  // Get the type information about the first element.
  mxArray* element = mxGetCell(*array, 0);
  mxClassID class_id = mxGetClassID(element);
  mwSize ndims = mxGetNumberOfDimensions(element);
  const mwSize* dims = mxGetDimensions(element);
  // Scan the rest of elements.
  bool mergeable = true;
  for (int i = 1; i < size; ++i) {
    element = mxGetCell(*array, i);
    mergeable &= class_id == mxGetClassID(element) &&
        ndims == mxGetNumberOfDimensions(element) &&
        memcmp(dims, mxGetDimensions(element), ndims * sizeof(mwSize)) == 0;
  }
  if (!mergeable)
    return;
  switch (class_id) {
    case mxDOUBLE_CLASS:
    case mxLOGICAL_CLASS:
      MergeNumericArrays(array);
      break;
    //case mxCHAR_CLASS: // Let's not merge strings to an N-D array.
    case mxCELL_CLASS:
      MergeCellArrays(array);
      break;
    case mxSTRUCT_CLASS:
      MergeStructArrays(array);
      break;
    default:
      if (mxIsClass(element, "bson.date"))
        MergeDateArrays(array);
      break;
  }
}

/** Convert a BSON array to an appropriate MxArray type.
 */
mxArray* ConvertBSONIteratorToMxArray(bson_iterator* it) {
  mxArray* element = NULL;
  bson_iterator iterator_copy = *it;
  // Check the array type.
  int object_size, array_type;
  const char** keys = NULL;
  ScanBSONObject(&iterator_copy, &object_size, &keys, &array_type);
  if (!keys)
    return NULL;
  // Convert.
  switch (array_type) {
    case mxDOUBLE_CLASS:
      element = ConvertBSONArrayToDoubleArray(it, object_size);
      break;
    case mxINT32_CLASS:
      element = ConvertBSONArrayToIntegerArray(it, object_size);
      break;
    case mxINT64_CLASS:
      element = ConvertBSONArrayToLongArray(it, object_size);
      break;
    case mxLOGICAL_CLASS:
      element = ConvertBSONArrayToLogicalArray(it, object_size);
      break;
    case mxCHAR_CLASS:
    case mxUINT8_CLASS:
      if (object_size == 1)
        element = ConvertNextToMxArray(it);
      else
        element = ConvertBSONArrayToCellArray(it, object_size);
      break;
    case mxCELL_CLASS:
      element = ConvertBSONArrayToCellArray(it, object_size);
      break;
    case mxSTRUCT_CLASS:
      element = ConvertBSONArrayToStructArray(it, object_size, keys);
      break;
    default:
      break;
  }
  mxFree(keys);
  // Merge a cell array to N-D array if possible.
  if (array_type == mxCELL_CLASS)
    TryMergeCellToNDArray(&element);
  return element;
}

/** Proceed to next and Convert a BSON value.
 */
static mxArray* ConvertNextToMxArray(bson_iterator* it) {
  mxArray* element = NULL;
  bson_type type = bson_iterator_next(it);
  switch (type) {
    case BSON_EOO:
      break;
    case BSON_DOUBLE:
      element = mxCreateDoubleScalar(bson_iterator_double(it));
      break;
    case BSON_STRING:
    case BSON_SYMBOL:
      element = mxCreateString(bson_iterator_string(it));
      break;
    case BSON_OBJECT:
    case BSON_ARRAY: {
      bson_iterator sub_iterator;
      bson_iterator_subiterator(it, &sub_iterator);
      element = ConvertBSONIteratorToMxArray(&sub_iterator);
      break;
    }
    case BSON_BINDATA: {
      int element_size = bson_iterator_bin_len(it);
      element = mxCreateNumericMatrix(1,
                                      element_size,
                                      mxUINT8_CLASS,
                                      mxREAL);
      memcpy(mxGetData(element), bson_iterator_bin_data(it), element_size);
      break;
    }
    case BSON_OID: {
      char oid_string[32];
      bson_oid_to_string(bson_iterator_oid(it), oid_string);
      element = mxCreateString(oid_string);
      break;
    }
    case BSON_BOOL:
      element = mxCreateLogicalScalar(bson_iterator_bool(it));
      break;
    case BSON_NULL:
      element = mxCreateDoubleMatrix(0, 0, mxREAL);
      break;
    case BSON_REGEX:
      element = mxCreateString(bson_iterator_regex(it));
      break;
    case BSON_CODE:
    case BSON_CODEWSCOPE:
      element = mxCreateString(bson_iterator_code(it));
      break;
    case BSON_INT:
      element = mxCreateDoubleScalar(bson_iterator_int(it));
      break;
    case BSON_DATE:{
      bson_date_t date_value = bson_iterator_date(it);
      mxArray* date_number = mxCreateDoubleScalar(
          ((double)date_value / 86400.0) + 719529);
      mexCallMATLAB(1, &element, 1, &date_number, "bson.date");
      mxDestroyArray(date_number);
      break;
    }
    case BSON_TIMESTAMP: {
      time_t time_value = bson_iterator_time_t(it);
      mxArray* date_number = mxCreateDoubleScalar(
          ((double)time_value / 86400.0) + 719529);
      mexCallMATLAB(1, &element, 1, &date_number, "bson.date");
      mxDestroyArray(date_number);
      break;
    }
    case BSON_LONG:
      element = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
      *(int64_t*)mxGetData(element) =
        bson_iterator_long(it);
      break;
    //case BSON_UNDEFINED:
    //case BSON_DBREF:
    default:
      break;
  }
  return element;
}

bool ConvertMxArrayToBSON(const mxArray* input, int flags, bson* output) {
  if (flags & BSON_FLAG_QUERY_MODE)
    bson_init_as_query(output);
  else
    bson_init(output);
  if (!ConvertArrayToBSON(input, NULL, output)) {
    bson_destroy(output);
    return false;
  }
  return bson_finish(output) == BSON_OK;
}

bool ConvertBSONToMxArray(const bson* input, mxArray** output) {
  bson_iterator it;
  bson_iterator_init(&it, input);
  *output = ConvertBSONIteratorToMxArray(&it);
  return *output != NULL;
}
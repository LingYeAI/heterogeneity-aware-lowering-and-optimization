//===- odla_ops_nn.h ------------------------------------------------------===//
//
// Copyright (C) 2019-2020 Alibaba Group Holding Limited.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================

#ifndef _ODLA_OPERATOR_OPS_NN_H_
#define _ODLA_OPERATOR_OPS_NN_H_

#include <ODLA/odla_common.h>
#include <ODLA/odla_value.h>

/*! \file
 * \details This file defines the ODLA neural network related operators.
 */

#ifdef __cplusplus
extern "C" {
#endif

//! \brief Memory layout of data
typedef enum {
  ODLA_CHANNELS_FIRST, /*!< layout channel dimension prior to spatial dimension
                        */
  ODLA_CHANNELS_LAST,  /*!< layout channel dimension prior to spatial dimension
                        */
  ODLA_SIO,            /*!< spatial dimension, input and output channels */
  ODLA_SOI,            /*!< spatial first, output and input channels */
  ODLA_OIS, /*!< output channls, input channels and spatial dimensions */
  ODLA_IOS, /*!< input channles, output channels and spatial dimensions */
} odla_memory_layout;

//! \brief RNN direction
typedef enum {
  ODLA_RNN_FORWARD,       /*!< forward */
  ODLA_RNN_REVERSE,       /*!< reverse */
  ODLA_RNN_BIDIRECTIONAL, /*!< both */
} odla_rnn_direction;

//! \brief RNN outputs
typedef enum {
  ODLA_RNN_NO_STATE,          /*!< default output */
  ODLA_RNN_HIDDEN_STATE,      /*!< output hidden state */
  ODLA_RNN_CELL_STATE,        /*!< output cell state */
  ODLA_RNN_HIDDEN_CELL_STATE, /*!< output both hidden and cell states */
} odla_rnn_outputs;

//! \brief Avgerage pooling
/*!
  AveragePool computes the average pooling across the \p input according to
  the window size, strides and paddings.

  \param input the input value
  \param input_layout the memory layout of input
  \param window_dims the spatial dimension of window
  \param strides the stride along each spatial axis
  \param paddings_front paddings applied to the start of each spatial dimension
  \param paddings_back paddings applied to the end of each spatial dimension
  \param output_dims the optional output shape (can be undefined)
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL odla_AveragePool(
    odla_value input, odla_memory_layout input_layout,
    const odla_uint32* window_dims, const odla_uint32* strides,
    const odla_uint32* paddings_front, const odla_uint32* paddings_back,
    odla_value_shape output_dims, const odla_value_id value_id);

/*!
  Local Response Normalization normalizes over local regions on \p input
  The result can then be scaled and biased.

  \param input the input value.
  \param input_layout the memory layout of input.
  \param window_size the size of the local region.
  \param alpha the scalaing factor.
  \param beta the exponent.
  \param bias the offset.
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_LRN(odla_value input, odla_memory_layout input_layout,
         odla_int32 window_size, odla_float32 alpha, odla_float32 beta,
         odla_float32 bias, const odla_value_id value_id);

//! \brief Compute batch normalization

/*!
  BatchNormalization computes the batch normalization on \p input
  as y = (x - mean) / sqrt(var + epsilon).
  The result can then be scaled and biased.
  If \p scale is not null, it will be applied. Otherwise,
  scalar value \p scalar_scale will be applied.
  Similar to \p offset and \p scalar_offset.

  \param input the input value
  \param input_layout the memory layout of input
  \param mean the mean value
  \param var the variance value
  \param epsilon the epsilon
  \param scale optional scale vector (can be NULL). If not NULL,
         it must be a vector with the same size as number of channels.
  \param offset optional offset vector (can be NULL). If not NULL,
         it must be a vector with the same size as number of channels.
  \param scalar_scale If `scale` is NULL, this value will be used.
  \param scalar_offset if `offset` is NULL, this value will be used.
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL odla_BatchNormalization(
    odla_value input, odla_memory_layout input_layout, odla_value mean,
    odla_value var, odla_float32 epsilon, odla_value scale, odla_value offset,
    odla_float32 scalar_scale, odla_float32 scalar_offset,
    const odla_value_id value_id);

//! \brief N-dimensional Convolution
/*!
  Conv computes convolution of \p input and \p kernel based on the strides,
  dialiations, padding and group.
  The result can be added with \p bias optionally.

  \param input the input value
  \param input_layout the memory layout of input
  \param group number of groups input and output channels are divided into
  \param kernel the kernel value
  \param kernel_layout the memory layout of kernel
  \param strides the stride along each spatial axis
  \param dilations dilations (same number of values as `strides`)
  \param paddings_front paddings applied to the start of each spatial dim
  \param paddings_back paddings applied to the end of each spatial dim
  \param bias optional bias. NULL if not needed.
  \param output_dims the optional output shape (can be undefined)
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_Conv(odla_value input, odla_memory_layout input_layout, odla_uint32 group,
          odla_value kernel, odla_memory_layout kernel_layout,
          const odla_uint32* strides, const odla_uint32* dilations,
          const odla_uint32* paddings_front, const odla_uint32* paddings_back,
          odla_value bias, odla_value_shape output_dims,
          const odla_value_id value_id);

//! \brief N-dimensional Deconvolution
/*!
  DeConv computes the deconvolution (transposed convolution) based on the
  strides, dialiations, padding and group. The result can be added with \p bias
  optionally.

  \param input the input value
  \param input_layout the memory layout of input
  \param group number of groups input and output channels are divided into
  \param kernel the kernel value
  \param kernel_layout the storage layout of kernel in memory
  \param strides the stride along each spatial axis
  \param dilations dilations (same number of values as `strides`)
  \param paddings_front paddings applied to the start of each spatial dim
  \param paddings_back paddings applied to the end of each spatial dim
  \param bias optional bias. NULL if not needed.
  \param output_dims the optional output shape (can be undefined)
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_DeConv(odla_value input, odla_memory_layout input_layout,
            odla_uint32 group, odla_value kernel,
            odla_memory_layout kernel_layout, const odla_uint32* strides,
            const odla_uint32* dilations, const odla_uint32* paddings_front,
            const odla_uint32* paddings_back, odla_value bias,
            odla_value_shape output_dims, const odla_value_id value_id);

//! \brief Elu activation
/*!
  Elu computes the elu activaion as y = x < 0 ? alpha * (exp(x) - 1) : x

  \param input the input value
  \param alpha the coefficient
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_Elu(odla_value input, odla_float32 alpha, const odla_value_id value_id);

//! \brief Compute a one-layer GRU
/*!
  GRU computes one-layer GRU. The output values are determined by \p outputs.

  \param input the input value
  \param weight_dims the dims of weights
  \param W the weights for gates. Assuming layout of [in, out, forget, cell]
  \param R the recurrence weight
  \param B the optional bias
  \param seq_len the sequence length
  \param hidden_size the size of hidden neurons
  \param direction the directon of network
  \param outputs speicify needed option outputs
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_values ODLA_API_CALL
odla_GRU(odla_value input, odla_value_shape weight_dims, odla_value W,
         odla_value R, odla_value B, odla_uint32 seq_len,
         odla_int32 hidden_size, odla_rnn_direction direction,
         odla_rnn_outputs outputs, const odla_value_id value_id);

//! \brief HardSigmoid activation
/*!
  HardSigmoid computes the hard sigmoid function as y = max(0, min( 1, alpha * x
  + beta)).

  \param input the input value
  \param alpha value of alpha
  \param beta value of beta
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_HardSigmoid(odla_value input, odla_float32 alpha, odla_float32 beta,
                 const odla_value_id value_id);

//! \brief Group Normalization
/*!
  GroupNormalization normalizes the features within channel groups.
  \param input the input value
  \param input_layout the memory layout of input
  \param groups the number of groups
  \param epsilon the epsilon
  \param scale optional scale value (can be NULL)
  \param offset optional offset value (Default is NULL)
  \param scalar_scale If `scale` is NULL, this value will be used.
  \param scalar_offset if `offset` is NULL, this value will be used.
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL odla_GroupNormalization(
    odla_value input, odla_memory_layout input_layout, odla_int32 groups,
    odla_float32 epsilon, odla_value scale, odla_value offset,
    odla_float32 scalar_scale, odla_float32 scalar_offset,
    const odla_value_id value_id);

//! \brief Instance Normalization
/*!
  InstanceNormalization computes the instance normalization as \n
  y = scale * (x - mean) / sqrt(variance + epsilon) + bias \n
  \p mean and \p var are per channel per instance.

  \param input the input value
  \param input_layout the memory layout of input
  \param mean the mean value
  \param var the variance value
  \param epsilon the epsilon
  \param scale optional scale value (can be NULL)
  \param offset optional offset value (Default is NULL)
  \param scalar_scale If `scale` is NULL, this value will be used.
  \param scalar_offset if `offset` is NULL, this value will be used.
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL odla_InstanceNormalization(
    odla_value input, odla_memory_layout input_layout, odla_value mean,
    odla_value var, odla_float32 epsilon, odla_value scale, odla_value offset,
    odla_float32 scalar_scale, odla_float32 scalar_offset,
    const odla_value_id value_id);

//! \brief LeakyRelu activation
/*!
  LeakyRelu computes the LeakyRelu activation as y = x < 0 ? alpha * x : x

  \param input the input value
  \param alpha the alpha value
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL odla_LeakyRelu(
    odla_value input, odla_float32 alpha, const odla_value_id value_id);

//! \brief LogSoftmax activation
/*!
  LogSoftmax computes the log softmax activation as y = x -
  log(reduce_sum(exp(x), axis))

  \param input the input value
  \param axis the dimension it will perform on
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL odla_LogSoftmax(
    odla_value input, odla_int32 axis, const odla_value_id value_id);

//! \brief LSTM
/*!
  LSTM computes one-layer LSTM.

  \param input the input value
  \param weight_dims the dims of weights
  \param W the weights for gates. Assuming layout of [in, out, forget, cell]
  \param R the recurrence weight
  \param B the optional bias
  \param seq_len the sequence length
  \param hidden_size the size of hidden neurons
  \param direction the directon of network
  \param outputs speicify needed option outputs
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_values ODLA_API_CALL
odla_LSTM(odla_value input, odla_value_shape weight_dims, odla_value W,
          odla_value R, odla_value B, odla_uint32 seq_len,
          odla_int32 hidden_size, odla_rnn_direction direction,
          odla_rnn_outputs outputs, const odla_value_id value_id);

//! \brief Max Pooling
/*!
  MaxPool computes the max pooling across the \p input according to the window
  size, strides and paddings.

  \param input the input value
  \param input_layout the memory layout of input
  \param window_dims the spatial dimension of window
  \param strides the stride along each spatial axis
  \param paddings_front paddings applied to the start of each spatial dim
  \param paddings_back paddings applied to the end of each spatial dim
  \param output_dims the optional output shape (can be undefined)
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL odla_MaxPool(
    odla_value input, odla_memory_layout input_layout,
    const odla_uint32* window_dims, const odla_uint32* strides,
    const odla_uint32* paddings_front, const odla_uint32* paddings_back,
    odla_value_shape output_dims, const odla_value_id value_id);

//! \brief Non-Max Suppression
/*!
  NMS applies the Non-max suppression on \p boxes that have overlap above
  \p iou_threshold.

  \param boxes the coordinates of boxes
  \param scores the scores
  \param max_num_outputs the maximum number of outputs
  \param iou_threshold the threshold of IoU
  \param score_threhold the threshold of scores
  \param output_value_type the output value type (an integer value type)
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_NMS(odla_value boxes, odla_value scores, odla_uint32 max_num_outputs,
         odla_float32 iou_threshold, odla_float32 score_threshold,
         odla_value_type output_value_type, const odla_value_id value_id);

//! \brief PRelu activation
/*!
  PRelu computes the PRelu activation as y = x < 0 ? slope * x : x

  \param input the input value
  \param slope the slope value
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_PRelu(odla_value input, odla_value slope, const odla_value_id value_id);

//! \brief Relu activateion
/*!
  Relu computes the relu activation as y = x < 0 : 0 ? x

  \param input the input value
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_Relu(odla_value input, const odla_value_id value_id);

//! \brief Region of Interest align
/*!
  ROIAlign computes the corresponding data from \p input using \p rois
  via bilinear interpolation.

  \param input the input value
  \param input_layout the memory layout of input
  \param rois the ROIs to pool. Dims of [num of ROIs, 4]
  \param sampling_ratio number of sampling points in the interpolation grid
  \param scale the scale factor to translate the ROI coordinates when pooling
  \param output_dim output dimension
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_ROIAlign(odla_value input, odla_memory_layout input_layout,
              odla_value rois, odla_int32 sampling_ratio, odla_float32 scale,
              odla_value_shape output_dim, const odla_value_id value_id);

//! \brief ROI max pooling
/*!
  ROIMaxPool Computes corresponding data from \p input using \p rois
  via max pooling.

  \param input the input value
  \param input_layout the memory layout of input
  \param rois the ROIs to pool. Dims of [num of ROIs, 4]
  \param win_h the height of the pooling window
  \param win_w the width of the pooling window
  \param scale the scale factor to translate the ROI coordinates when pooling
  \param output_dim output dimension
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL odla_ROIMaxPool(
    odla_value input, odla_memory_layout input_layout, odla_value rois,
    odla_int32 win_h, odla_int32 win_w, odla_float32 scale,
    odla_value_shape output_dim, const odla_value_id value_id);

//! \brief Selu activation
/*!
  Selu computes the selu activation as
  y = x <= 0 ? gamma * (alpha * e^x - alpha) : gamma * x

  \param input the input value
  \param alpha the alpha value
  \param gamma the gamma value
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_Selu(odla_value input, odla_float32 alpha, odla_float32 gamma,
          const odla_value_id value_id);

//! \brief Sigmoid activation
/*!
  Sigmoid computes the sigmoid activation as y = 1 / (1 + exp(-x))

  \param input the input value
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_Sigmoid(odla_value input, const odla_value_id value_id);

//! \brief Softmax activation
/*!
  Softmax computes the softmax activation as
  y = exp(x) / reduce_sum(exp(x), axis)

  \param input the input value
  \param axis the dimension it will perform on
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_Softmax(odla_value input, odla_int32 axis, const odla_value_id value_id);

//! \brief Tanh activation
/*!
  Tanh computes the tanh activation as (1 - e^{-2x})/(1 + e^{-2x})

  \param input the input value
  \param value_id a unique value id (can be NULL)

  \return odla_value
*/
extern ODLA_API_EXPORT odla_value ODLA_API_CALL
odla_Tanh(odla_value input, const odla_value_id value_id);

//! \brief Find Top-K elements
/*!
  TopK returns the top-K largest or smallest elements and index

  \param input the input value
  \param K the number of top values to be returned
  \param largest indicates largest (true) or smallest values to retrieve
  \param sorted indicates if the results are sorted or in original order
  \param dimension on which to do the sort
  \param output_value_type the output 0 type
  \param output_value_index_type the output 1 type
  \param value_ids an array of value ids (can be NULL)

  \return odla_values
*/
extern ODLA_API_EXPORT odla_values ODLA_API_CALL odla_TopK(
    odla_value input, odla_uint32 K, odla_bool largest, odla_bool sorted,
    odla_uint32 axis, odla_value_type output_value_type,
    odla_value_type output_value_index_type, const odla_value_ids value_ids);

#ifdef __cplusplus
} // C extern
#endif

#endif // _ODLA_OPERATOR_OPS_NN_H_

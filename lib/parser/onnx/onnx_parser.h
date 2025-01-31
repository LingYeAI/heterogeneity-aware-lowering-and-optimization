//===- onnx_parser.h --------------------------------------------*- C++ -*-===//
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

#ifndef HALO_LIB_PARSER_ONNX_ONNXPARSER_H_
#define HALO_LIB_PARSER_ONNX_ONNXPARSER_H_

#include <functional>
#include <stack>
#include <unordered_map>

#include "halo/lib/ir/ir_builder.h"
#include "halo/lib/parser/parser.h"

namespace onnx {
class AttributeProto;
class GraphProto;
class NodeProto;
class TensorProto;
class ValueInfoProto;
} // namespace onnx

namespace halo {
/// Record string based attributes
class ONNXAttrs {
 public:
  template <typename T>
  using AttrMap = std::unordered_map<std::string, T>;

  ONNXAttrs() = delete;
  explicit ONNXAttrs(const onnx::NodeProto& node_def);

  template <typename T>
  bool Process(const std::string& key, T*);

  ONNXAttrs(const ONNXAttrs&) = delete;
  ONNXAttrs& operator=(const ONNXAttrs&) = delete;

 private:
  AttrMap<const onnx::AttributeProto&> attr_map_;
};

/// Parser for ONNX
class ONNXParser : public Parser {
  class Scope {
    using ir_map = std::unordered_map<std::string, Value>;

   public:
    bool Contains(const std::string& name);
    Value Find(const std::string& name);
    void Insert(const onnx::TensorProto& tensor, const Value& def);
    void Insert(const std::string& name, const Value& def);
    Scope* GetParent() const { return parent_; }
    Scope* CreateScope();

   private:
    ir_map inst_name_to_ptr_;
    std::vector<std::unique_ptr<Scope>> sub_scopes_;

   private:
    Scope* parent_ = nullptr;
  };

 public:
  explicit ONNXParser(){};
  Status Parse(Function* function, const std::vector<std::string>& file_list,
               const armory::Opts& opts) override;
  Status Parse(Function* function, const std::vector<const char*>& buffers,
               const std::vector<size_t>& buffer_sizes) override;
  Status Parse(Function* function,
               const std::vector<const void*>& model_defs) override;
  Status Parse(BasicBlock* bb, const onnx::GraphProto& graph_def,
               const armory::Opts& opts);

  ~ONNXParser();

  template <typename T>
  static Tensor<T> ProcessTensor(const onnx::TensorProto& tensor_proto);

  ONNXParser(const ONNXParser&) = delete;
  ONNXParser& operator=(const ONNXParser&) = delete;

  static halo::DataType ProcessDataType(int data_type);
  static void WriteCSVReport(const onnx::NodeProto& cur_node, std::ostream& os);

 private:
  void RegisterOp();
  Status ConvertToHaloIR(const onnx::GraphProto& graph_def);
  Status ConvertOneNode(IRBuilder* ir_builder, const onnx::NodeProto& node_def);
  IRObject* ConvertConstNode(ConstantBuilder* c_builder,
                             const onnx::TensorProto& tensor_def);
  IRObject* ConvertConstNode(ConstantBuilder* c_builder,
                             const onnx::TensorProto& tensor_def,
                             const std::string& name);
  Status ConvertConstNode(ConstantBuilder* c_builder,
                          const onnx::NodeProto& cur_node);
  Status ConvertDummyNode(IRBuilder* ir_builder,
                          const onnx::NodeProto& cur_node);
  Status ConvertLoopNode(IRBuilder* ir_builder,
                         const onnx::NodeProto& cur_node);
  Status ConvertPlaceholderNode(ArgumentBuilder* arg_builder,
                                const onnx::ValueInfoProto& value_info_def);
  Status ConvertSubPlaceholderNode(ArgumentBuilder* arg_builder,
                                   const onnx::ValueInfoProto& value_info_def);
  std::vector<Def> GetInputOperands(const onnx::NodeProto& node_def);
  void InsertIDToInstMap(const onnx::NodeProto& node_def, IRObject* inst);
  static Type GetType(const onnx::ValueInfoProto& value_info_def);

/// create node function auto generatered by tablegen
#include "onnx_convert.h.inc"

 private:
  std::unique_ptr<BasicBlockBuilder> bb_builder_;
  std::unique_ptr<IRBuilder> ir_builder_;
  std::unique_ptr<ArgumentBuilder> arg_builder_;
  std::unique_ptr<ConstantBuilder> c_builder_;
  armory::Opts opts_;
  using CallBack =
      std::function<Status(IRBuilder* ir_builder, const onnx::NodeProto&)>;
  std::unordered_map<std::string, CallBack> func_lists_;
  std::stack<Type> loop_arg_types_;
  Scope root_scope_;
  Scope* curr_scope_ = &root_scope_;
}; // namespace halo

} // namespace halo

#endif // HALO_LIB_PARSER_ONNX_ONNXPARSER_H_

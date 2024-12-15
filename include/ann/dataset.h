/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   dataset.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 3:59 PM
 */

#ifndef DATASET_H
#define DATASET_H
#include "ann/xtensor_lib.h"
using namespace std;

template <typename DType, typename LType>
class DataLabel {
 private:
  xt::xarray<DType> data;
  xt::xarray<LType> label;
  xt::svector<unsigned long> data_shape, label_shape;

 public:
  DataLabel(xt::xarray<DType> data, xt::xarray<LType> label)
      : data(data), label(label) {}
  xt::xarray<DType> getData() const { return data; }
  xt::xarray<LType> getLabel() const { return label; }
};

template <typename DType, typename LType>
class Batch {
 private:
  xt::xarray<DType> data;
  xt::xarray<LType> label;

 public:
  Batch(xt::xarray<DType> data, xt::xarray<LType> label)
      : data(data), label(label) {}
  virtual ~Batch() {}
  xt::xarray<DType>& getData() { return data; }
  xt::xarray<LType>& getLabel() { return label; }
};

template <typename DType, typename LType>
class Dataset {
 private:
 public:
  Dataset() {};
  virtual ~Dataset() {};

  virtual int len() = 0;
  virtual DataLabel<DType, LType> getitem(int index) = 0;
  virtual xt::svector<unsigned long> get_data_shape() = 0;
  virtual xt::svector<unsigned long> get_label_shape() = 0;
};

//////////////////////////////////////////////////////////////////////
template <typename DType, typename LType>
class TensorDataset : public Dataset<DType, LType> {
 private:
  xt::xarray<DType> data;
  xt::xarray<LType> label;
  xt::svector<unsigned long> data_shape, label_shape;

 public:
  TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label) {
    // TODO implement
    this->data = std::move(data);
    this->label = std::move(label);
    this->data_shape = static_cast<xt::svector<long unsigned int>>(this->data.shape());
    this->label_shape = static_cast<xt::svector<long unsigned int>>(this->label.shape());

  }

  int len() override {
    // TODO implement
    return static_cast<int>(this->data.shape()[0]);
  }

  DataLabel<DType, LType> getitem(int index) override {
    // TODO implement
    if (index < 0 || index >= this->len()) {
    throw std::out_of_range("Index is out of range!");
    }

    xt::xarray<DType> data_item = xt::view(this->data, index);
    xt::xarray<LType> label_item;

    if (this->label.dimension() > 0) {
      label_item = xt::view(this->label, index);
    } else {
      label_item = this->label;
    }

    return DataLabel<DType, LType>(std::move(data_item), std::move(label_item));
  }

  xt::svector<unsigned long> get_data_shape() { return data_shape; }

  xt::svector<unsigned long> get_label_shape() { return label_shape; }
};

#endif /* DATASET_H */
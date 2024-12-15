/* 
 * File:   dataloader.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 4:01 PM
 */

#ifndef DATALOADER_H
#define DATALOADER_H
#include "ann/xtensor_lib.h"
#include "ann/dataset.h"

using namespace std;

template <typename DType, typename LType>
class DataLoader {
 private:
  Dataset<DType, LType>* ptr_dataset;
  int batch_size;
  bool shuffle;
  bool drop_last;
  int m_seed;
  vector<size_t> indices;
  size_t current_index;

 public:
  DataLoader(Dataset<DType, LType>* ptr_dataset, int batch_size,
             bool shuffle = true, bool drop_last = false, int seed = -1)
      : ptr_dataset(ptr_dataset),
        batch_size(batch_size),
        shuffle(shuffle),
        drop_last(drop_last),
        m_seed(seed),
        current_index(0) {
    indices.resize(ptr_dataset->len());
    for (size_t i = 0; i < indices.size(); ++i) {
      indices[i] = i;
    }
    if (shuffle) {
      if (seed >= 0) {
        xt::random::seed(m_seed);
      }
      xt::random::shuffle(indices);
    }
  }

  virtual ~DataLoader() = default;

  class Iterator {
   private:
    DataLoader* loader;
    size_t current_index;

   public:
    Iterator(DataLoader* loader, size_t start_index)
        : loader(loader), current_index(start_index) {}

    Iterator& operator=(const Iterator& iterator) {
      loader = iterator.loader;
      current_index = iterator.current_index;
      return *this;
    }

    Iterator& operator++() {
      current_index += loader->batch_size;
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator!=(const Iterator& other) const {
      return current_index != other.current_index;
    }

    Batch<DType, LType> operator*() const {
      size_t remaining = loader->ptr_dataset->len() - current_index;
      size_t actual_batch_size = min(loader->batch_size, remaining);
      
      vector<size_t> batch_indices(loader->indices.begin() + current_index,
                                   loader->indices.begin() + current_index + actual_batch_size);
      
      vector<xt::xarray<DType>> batch_data;
      vector<xt::xarray<LType>> batch_labels;
      
      for (size_t idx : batch_indices) {
        DataLabel<DType, LType> item = loader->ptr_dataset->getitem(idx);
        batch_data.push_back(item.getData());
        batch_labels.push_back(item.getLabel());
      }
      
      xt::xarray<DType> data = xt::stack(batch_data);
      xt::xarray<LType> labels = xt::stack(batch_labels);
      
      return Batch<DType, LType>(data, labels);
    }
  };

  Iterator begin() {
    return Iterator(this, 0);
  }

  Iterator end() {
    size_t end_index = ptr_dataset->len();
    if (drop_last) {
      end_index = (end_index / batch_size) * batch_size;
    }
    return Iterator(this, end_index);
  }
};

#endif /* DATALOADER_H */
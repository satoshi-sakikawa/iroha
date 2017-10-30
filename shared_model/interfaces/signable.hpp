/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IROHA_SIGNABLE_HPP
#define IROHA_SIGNABLE_HPP

#include <boost/functional/hash.hpp>
#include <unordered_set>
#include "interfaces/common_objects/signature.hpp"
#include "interfaces/hashable.hpp"
#include "interfaces/polymorphic_wrapper.hpp"

namespace shared_model {
  namespace interface {

    /**
     * Interface provides signatures and adding them to model object
     * @tparam Model - your new style model
     * Architecture note: we inherit Signable from Hashable with following
     * assumption - all Signable objects are signed by hash value.
     */
    template <typename Model, typename OldModel>
    class Signable : public Hashable<Model, OldModel> {
     public:
      /// Type of transaction signature
      using SignatureType = detail::PolymorphicWrapper<Signature>;

      /// Type of set of signatures
      /**
       * Note: we can't use const SignatureType due to unordered_set
       * limitations: it requires to have write access for elements for some
       * internal operations.
       */
      using SignatureSetType = std::unordered_set<SignatureType>;

      /**
       * @return attached signatures
       */
      virtual const SignatureSetType &signatures() const = 0;

      /**
       * Attach signature to object
       * @param signature - signature object for insertion
       * @return true, if signature was added
       */
      virtual bool addSignature(const SignatureType &signature) = 0;

      /// Type of timestamp
      using TimestampType = uint64_t;

      /**
       * @return time of creation
       */
      virtual const TimestampType &createdTime() const = 0;
    };
  }  // namespace interface
}  // namespace shared_model

namespace std {
  /**
   * Hash for SignatureSetType. It's required because std::unordered_set uses
   * hash inside and it should be declared explicitly for user-defined types.
   */

  using SigWrapper = shared_model::detail::
      PolymorphicWrapper<shared_model::interface::Signature>;

  template <>
  struct hash<SigWrapper> {
    size_t operator()(const SigWrapper &sig) const {
      std::size_t seed = 0;
      boost::hash_combine(seed, sig->publicKey().blob());
      boost::hash_combine(seed, sig->signedHash().blob());
      return seed;
    }
  };
}

#endif  // IROHA_SIGNABLE_HPP

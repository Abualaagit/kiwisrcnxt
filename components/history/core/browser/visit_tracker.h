// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_HISTORY_CORE_BROWSER_VISIT_TRACKER_H__
#define COMPONENTS_HISTORY_CORE_BROWSER_VISIT_TRACKER_H__

#include <map>
#include <vector>

#include "components/history/core/browser/history_types.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace history {

// Tracks history transitions between pages. The history backend uses this to
// link up page transitions to form a chain of page visits, and to set the
// transition type properly.
//
// This class is not thread safe.
class VisitTracker {
 public:
  VisitTracker();
  VisitTracker(const VisitTracker&) = delete;
  VisitTracker& operator=(const VisitTracker&) = delete;
  ~VisitTracker();

  void AddVisit(ContextID context_id,
                int nav_entry_id,
                const GURL& url,
                VisitID visit_id);

  void RemoveVisitById(VisitID visit_id);

  // Removes all visits.
  void Clear();

  bool IsEmpty() const { return contexts_.empty(); }

  // Removes all visits added with the specified ContextID.
  void ClearCachedDataForContextID(ContextID context_id);

  // Querying ------------------------------------------------------------------

  // Returns the VisitID last supplied to AddVisit() matching the supplied
  // parameters. Returns 0 if there is no matching visit.
  VisitID GetLastVisit(ContextID context_id,
                       int nav_entry_id,
                       const GURL& url);

 private:
  struct Transition {
    GURL url;          // URL that the event happened to.
    int nav_entry_id;  // ID of the navigation entry.
    VisitID visit_id;  // Visit ID generated by history.
  };

  // Compares Transition objects by visit_id.
  struct TransitionVisitIdComparator {
    bool operator()(const Transition& a, const Transition& b) {
      return a.visit_id < b.visit_id;
    }
  };

  struct VisitIdRange {
    VisitID min_id;
    VisitID max_id;
  };

  using TransitionList = std::vector<Transition>;

  // Expires oldish items in the given transition list. This keeps the list
  // size small by removing items that are unlikely to be needed, which is
  // important for GetReferrer which does brute-force searches of this list.
  void CleanupTransitionList(TransitionList* transitions);

  // Returns true if AddVisit() was called with increasing visit-ids. In other
  // words, if this is true then TransitionList are sorted by visit-id. The
  // return value is undefined if IsEmpty() is true.
  bool are_transition_lists_sorted() const {
    return visit_id_range_if_sorted_.has_value();
  }

  TransitionList::const_iterator FindTransitionListIteratorByVisitId(
      const TransitionList& transitions,
      const Transition& transition_for_search);

  // Maps render view hosts to lists of recent transitions.
  std::map<ContextID, TransitionList> contexts_;

  // Represents the range of visit ids encountered. This has a value if
  // AddVisit() was called with increasing ids, which is generally the case. If
  // this does not have a value, then AddVisit() was not called with
  // increasing ids and each TransitionList is not sorted.
  //
  // This is not recalculated during removal as it would be expensive and is
  // generally unnecessary (because AddVisit() is typically called with
  // increasing visit-ids).
  absl::optional<VisitIdRange> visit_id_range_if_sorted_;
};

}  // namespace history

#endif  // COMPONENTS_HISTORY_CORE_BROWSER_VISIT_TRACKER_H__

#ifndef NFD_DAEMON_FW_FIREWALL_FIREWALL_STRATEGY_HPP
#define NFD_DAEMON_FW_FIREWALL_FIREWALL_STRATEGY_HPP

#include "strategy.hpp"
#include "process-nack-traits.hpp"
#include "retx-suppression-exponential.hpp"
#include <string>
#include <interest-filter.hpp>

namespace nfd {
namespace fw {

/** \brief a forwarding strategy that forwards Interest to all FIB nexthops
 */
class FirewallStrategy : public Strategy
                        , public ProcessNackTraits<FirewallStrategy>
{
public:
  explicit
  FirewallStrategy(Forwarder& forwarder, const Name& name = getStrategyName());

  static const Name&
  getStrategyName();

  int
  checkInterest(const Interest& interest);

  void
  afterReceiveInterest(const Face& inFace, const Interest& interest,
                       const shared_ptr<pit::Entry>& pitEntry) override;
  
  void
  _afterReceiveInterest(const Face& inFace, const Interest& interest,
                                        const shared_ptr<pit::Entry>& pitEntry);
  
  void
  afterReceiveNack(const Face& inFace, const lp::Nack& nack,
                   const shared_ptr<pit::Entry>& pitEntry) override;

private:
  friend ProcessNackTraits<FirewallStrategy>;
  RetxSuppressionExponential m_retxSuppression;
  const std::string BannedString = "red";



PUBLIC_WITH_TESTS_ELSE_PRIVATE:
  static const time::milliseconds RETX_SUPPRESSION_INITIAL;
  static const time::milliseconds RETX_SUPPRESSION_MAX;
};

} // namespace fw
} // namespace nfd

#endif // NDNSIM_EXAMPLES_TEST_FIREWALL_STRATEGY_HPP